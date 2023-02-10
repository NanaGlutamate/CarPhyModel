// ********************************************** //
// 原子模型最终应切换到Release模式下
// 编译生成后提交到模型库中
// ********************************************** //

#include <any>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <ctime>
#include <cstdint>
#include <regex>
#ifdef _WIN32
#include <Windows.h>
#include <atlbase.h>
#include <atlwin.h>
#else
#include <dlfcn.h>
#endif
#include <filesystem>

#include "csmodel_base.h"
#include "model_info.h"
#include "type_define_info.h"
#include "printutil.h"

namespace fs = std::filesystem;
using namespace config;

typedef CSModelObject* (*CreateModelObjectFun)();
typedef void (*DestroyMemoryFun)(void* mem, bool is_array);

// 设置模型初始化参数
bool SetInitParamsToModel(CSModelObject*);

// 设置模型输入参数
bool SetInputParamsToModel(CSModelObject*, uint64_t);

// 获取模型的输出参数
bool GetOutputParamsFromModel(
  CSModelObject*, const ModelInfo&, const TypeDefineInfo&);

// 模型执行单步运算
bool Tick(CSModelObject*, double);

// time_point转字符串
std::string TimePointToStringMilliSeconds(
  std::chrono::time_point<std::chrono::system_clock>);

// 获得time_point数据结构
std::chrono::system_clock::time_point GetTimePoint(uint16_t, uint16_t,
    uint16_t, uint16_t, uint16_t, uint16_t);

// 获得当前时间字符串
std::string GetCurrentTimeStr(
  const std::chrono::system_clock::time_point&, uint64_t);

// 获得当前仿真持续时间字符串
std::string GetTimeDurationStr(uint64_t);

// 模型库文件名称
std::string LibraryName(const std::string&);

// 输出参数类型校验及数值打印
bool PrintOutputParams(
  std::unordered_map<std::string, std::any>*,
  const ModelInfo&, const TypeDefineInfo& 
);

int main(int argc, char *argv[]) {
#ifdef _WIN32
  system("chcp 65001");
#endif

  // 以下参数应根据实际测试需求进行设置

  // 仿真步长(ms)
  double sim_step = 50.0;
  // 仿真开始时间
  uint16_t year = 2022;
  uint16_t mon = 5;
  uint16_t day = 31;
  uint16_t hour = 18;
  uint16_t min = 59;
  uint16_t sec = 59;

  // 以下参数无需变更

  // 帧数
  uint64_t current_ticks = 0;
  // 仿真开始时间
  auto sim_start_time = GetTimePoint(year, mon, day, hour, min, sec);
  // 仿真持续时间
  uint64_t sim_current_duration = 0;
  
  std::string exe_dir_ = fs::current_path().generic_string();
  std::string solution_dir_(exe_dir_);
#ifdef _WIN32
  for (size_t i = 0; i < 4; i++) {
    solution_dir_ = solution_dir_.substr(0, solution_dir_.find_last_of('/'));
  }
#else
  for (size_t i = 0; i < 3; i++) {
    solution_dir_ = solution_dir_.substr(0, solution_dir_.find_last_of('/'));
  }
#endif

  // 加载模型描述文件
  CSModelInfo model_info_;
  if (!model_info_.ReadFile(solution_dir_ + 
      "/src/CarPhyModel/CarPhyModel.xml")) {
    std::cout << "模型描述文件解析失败" << std::endl;
    return false;
  }

  // 加载数据类型定义文件
  CSTypeDefineInfo type_define_info_;
  if (!type_define_info_.ReadFile(solution_dir_ +
      "/src/typedefine.xml")) {
    std::cout << "自定义数据类型定义文件解析失败" << std::endl;
    return false;
  }

  // 模型动态库文件名称
  std::string lib_name_ = LibraryName("CarPhyModel");
#ifdef _WIN32
  std::string lib_path_ = exe_dir_ + "\\models\\test-model-id\\" + lib_name_;
  lib_path_ = std::regex_replace(lib_path_, std::regex("/"), "\\");
#else
  std::string lib_path_ = exe_dir_ + "/models/test-model-id/" + lib_name_;
#endif

#ifdef _WIN32
  auto hmodule =
    LoadLibraryExA(lib_path_.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#else
  void* hmodule = dlopen(lib_path_.c_str(), RTLD_LAZY | RTLD_GLOBAL);
#endif
  if (!hmodule) {
    std::cout << "加载 " << lib_name_ << " 失败！" << std::endl;
    return false;
  }

#ifdef _WIN32
  auto create_obj_ =
    (CreateModelObjectFun)GetProcAddress(hmodule, "CreateModelObject");
  auto destroy_obj_ =
    (DestroyMemoryFun)GetProcAddress(hmodule, "DestroyMemory");
#else
  auto create_obj_ =
    (CreateModelObjectFun)dlsym(hmodule, "CreateModelObject");
  auto destroy_obj_ =
    (DestroyMemoryFun)dlsym(hmodule, "DestroyMemory");
#endif
  if (!create_obj_ || !destroy_obj_) {
#ifdef _WIN32
    if (!FreeLibrary(hmodule)) 
#else
    if (!dlclose(hmodule)) 
#endif
      std::cout << "释放动态库失败！！" << std::endl;
    return false;
  }

  // 模型实例
  CSModelObject *model_obj_ = create_obj_();
  if (nullptr == model_obj_) {
    std::cerr << "模型实例创建失败" << std::endl;
    return -1;
  }
  

  // 日志输出回调
  std::function<void(const std::string &, uint32_t)> log_cb_ =
  [](const std::string &msg_, uint32_t level) {
    std::string level_ = "TRACE";
    if (1 == level)
      level_ = "DEBUG";
    else if (2 == level)
      level_ = "INFO";
    else if (3 == level)
      level_ = "WARN";
    else if (4 == level)
      level_ = "ERROR";
    else if (5 == level)
      level_ = "CRITICAL ERROR";

    std::cout << "\t日志[" << level_ << "] " << msg_ << std::endl;
  };

  // 通用回调
  std::function<std::string(const std::string &,
                            const std::unordered_map<std::string, std::any> &)>
  com_cb_ =
    [&sim_step, &current_ticks, &sim_current_duration, &sim_start_time](
      const std::string &type_name_,
      const std::unordered_map<std::string, std::any> &value_)
  -> std::string {
    std::string return_msg_ = "";
    if ("Step" == type_name_) {
      // 返回仿真步长
      return_msg_ = std::to_string(sim_step);
    } else if ("CurrentTime" == type_name_) {
      // 返回当前仿真时间
      return_msg_ = GetCurrentTimeStr(sim_start_time, sim_current_duration);
    } else if ("TickCount" == type_name_) {
      // 返回当前帧数
      return_msg_ = std::to_string(current_ticks);
    } else if ("TimeDuration" == type_name_) {
      // 返回当前仿真持续时长
      return_msg_ = GetTimeDurationStr(sim_current_duration);
    } else if ("CreateEntity" == type_name_) {
      // 动态创建模型实体
      // TODO! 暂未实现
    } else if ("DirectWriteTopic" == type_name_) {
      // 直接发送主题数据
      // TODO! 暂未实现
    }
    return return_msg_;
  };

  model_obj_->SetLogFun(log_cb_);
  model_obj_->SetCommonCallBack(com_cb_);

  // 设置模型初始化信息
  if (!SetInitParamsToModel(model_obj_)) return -1;

  // 仿真运行
  while (1) {
    // 索取模型的输出参数
    if (!GetOutputParamsFromModel(model_obj_, model_info_.GetInfo(), 
                                  type_define_info_.GetInfo())) {
      break;
    }

    // 向模型设置输入参数
    if (!SetInputParamsToModel(model_obj_, current_ticks)) break;

    // 模型单步运算
    if (!Tick(model_obj_, sim_step)) break;

    current_ticks++;
    sim_current_duration += (uint64_t)sim_step;

    std::cout << "当前帧数:" << current_ticks << "\t" 
              << "仿真步长:" << sim_step << "ms\t" 
              << "仿真持续时长:" << GetTimeDurationStr(sim_current_duration) << "ms\t" 
              << "当前仿真时间" << GetCurrentTimeStr(sim_start_time, 
                  sim_current_duration) << std::endl << std::endl;
  }
  destroy_obj_(model_obj_, false);
  std::cout << std::endl << "仿真结束" << std::endl;

  return 0;
}

// 设置模型初始化参数
bool SetInitParamsToModel(CSModelObject *model_obj_) {
  // 以下参数由模型在资源库中的字段信息以及想定中记录的模型实例信息决定

  uint64_t id = 10001;
  uint16_t force_side_id = 1;
  std::string model_id_("test-model-id");
  std::string instance_name_("CarPhyModel-10001");

  // 设置模型初始化参数

  model_obj_->SetID(id);
  model_obj_->SetForceSideID(force_side_id);
  model_obj_->SetInstanceName(instance_name_);
  model_obj_->SetModelID(model_id_);

  // 初始化参数
  std::unordered_map<std::string, std::any> init_params_;

  // TODO! (需根据模型进行更新) 设置模型初始化参数

  // 例:
  // double test_speed = 150.0;
  // init_params_.emplace("SpeedKMH", test_speed);

  if (!model_obj_->Init(init_params_)) {
    std::cerr << "模型初始化失败" << std::endl;
    return false;
  }
  return true;
}

// TODO! (需根据模型进行更新) 设置模型输入参数
bool SetInputParamsToModel(CSModelObject *model_obj_, uint64_t current_ticks) {
  try {
    // 例1:
    // 在第3帧向模型传递一组输入参数
    // 假设模型接收MissileID和TargetPos输入参数
    // 其中TargetPos是一个包含三个float64型参数的结构体
    //
    // if (3 == current_ticks) {
    //   std::unordered_map<std::string, std::any> input_params_;
    //
    //   uint64_t missile_id = 1001;
    //   input_params_.emplace("MissileID", missile_id);
    //
    //   std::unordered_map<std::string, std::any> target_pos_ = {
    //     {"longitude", 110.0},
    //     {"latitude", 80.0},
    //     {"altitude", 0.0}
    //   };
    //   input_params_.emplace("TargetPos", target_pos_);
    //
    //   if (!model_obj_->SetInput(input_params_)) {
    //     std::cerr << "设置模型输入参数失败" << std::endl;
    //     return false;
    //   }
    //
    //   input_params_.clear();
    // }

    // 例2:
    // 在第10~20帧中每帧向模型传递两组输入参数
    // 输入参数为一个int64数组型参数
    //
    // if (10 <= current_ticks && 20 >= current_ticks) {
    //   std::unordered_map<std::string, std::any> input_params_;
    //
    //   std::vector<std::any> target_id_list_ = {
    //     (int64_t)2001, (int64_t)2002, (int64_t)2003
    //   };
    //   input_params_.emplace("TargetIdList", target_id_list_);
    //
    //   if (!model_obj_->SetInput(input_params_)) {
    //     std::cerr << "设置模型输入参数失败" << std::endl;
    //     return false;
    //   }
    //
    //   input_params_.clear();
    // }

  } catch (const std::exception &e) {
    std::cerr << "发生异常: " << e.what() << std::endl;
    return false;
  }

  return true;
}

// 获取模型的输出参数
bool GetOutputParamsFromModel(CSModelObject *model_obj_, 
                              const ModelInfo& model_info_,
                              const TypeDefineInfo& type_define_info_) {
  try {
    // 判断是否需销毁模型实例
    if (CSInstanceState::IS_DESTROYED == model_obj_->GetState()) {
      std::cout << "模型实例销毁" << std::endl;
      return false;
    }
    // 判断模型实例是否出现异常
    if (CSInstanceState::IS_ERROR == model_obj_->GetState()) {
      std::cerr << "模型实例内部出现错误" << std::endl;
      return false;
    }

    // 获取模型的输出参数
    std::unordered_map<std::string, std::any> *output_params_ =
      model_obj_->GetOutput();
    if (nullptr == output_params_) {
      std::cerr << "模型实例参数输出出现错误" << std::endl;
      return false;
    }

    // 校验输出参数类型并打印数值
    // 
    // 注意:
    // 模型描述文件发生变更时此函数很可能发生错误
    // 详情见PrintOutputParams函数注释
    if (!PrintOutputParams(output_params_, model_info_, type_define_info_)) {
      std::cerr << "输出参数打印出现错误" << std::endl;
      return false;
    }
  } catch (const std::exception &e) {
    std::cerr << "发生异常: " << e.what() << std::endl;
    return false;
  }
  return true;
}

// 执行单步解算
bool Tick(CSModelObject *model_obj_, double sim_step) {
  try {
    // 执行单步解算
    if (!model_obj_->Tick(sim_step)) {
      std::cerr << "模型单步解算失败" << std::endl;
      return false;
    }
  } catch (const std::exception &e) {
    std::cerr << "发生异常: " << e.what() << std::endl;
    return false;
  }
  return true;
}

std::string TimePointToStringMilliSeconds(
  std::chrono::time_point<std::chrono::system_clock>
  time_point) {
  std::time_t t = std::chrono::system_clock::to_time_t(time_point);
  std::tm tm2;
#ifdef _WIN32
  localtime_s(&tm2, &t);
#else
  localtime_r(&t, &tm2);
#endif
  char buf[100] = { 0 };
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm2);
  std::string str = buf;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
              time_point.time_since_epoch()).count();
  str.append(".");
  str.append(std::to_string(ms % 1000));
  return std::move(str);
}

std::chrono::system_clock::time_point GetTimePoint(uint16_t year, uint16_t mon,
    uint16_t day, uint16_t hour, uint16_t min, uint16_t sec) {
  std::tm tm{};
  tm.tm_year = year - 1900;
  tm.tm_mon = mon - 1;
  tm.tm_mday = day;
  tm.tm_hour = hour;
  tm.tm_min = min;
  tm.tm_sec = sec;
  tm.tm_isdst = 0;

  return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

std::string GetCurrentTimeStr(
  const std::chrono::system_clock::time_point& start_time_,
  uint64_t time_duration) {
  std::chrono::system_clock::time_point current_time = start_time_ +
      std::chrono::milliseconds((long long)time_duration);
  return TimePointToStringMilliSeconds(current_time);
}

std::string GetTimeDurationStr(uint64_t time_duration) {
  return std::to_string(time_duration);
}

// 模型库文件名称
std::string LibraryName(const std::string& library_name_) {
  std::string library_cvt_name_;
#ifdef _WIN32
  library_cvt_name_ = library_name_ + ".dll";
#else
  library_cvt_name_ = "lib" + library_name_ + ".so";
#endif
  return library_cvt_name_;
}

// 输出参数类型校验及数值打印
// 
// 注意:
// 若模型描述文件中记录的参数名称或参数数据类型发生变更
// 则本函数可能出现数据类型转换错误或打印错误等情况
//
// 可能的原因:
// 1.模型源码建模功能上的信息与模型描述文件不一致
// 2.自定义数据类型文件上的信息与实际使用的数据结构不一致
// 
// 建议:
// 若模型描述文件中记录的参数名称或参数数据类型发生变更
// 请确保模型描述文件、自定义数据类型文件中记录的信息与模型的代码实现保持一致
// 
bool PrintOutputParams(
  std::unordered_map<std::string, std::any> *output_params_,
  const ModelInfo& model_info_,
  const TypeDefineInfo& type_define_info_
) {
  // 是否为输出参数
  auto is_output_param_ = [](const std::string& str) -> bool {
    size_t start = 0, end = 0;
    while ((start = str.find_first_not_of(',', end)) !=
           std::string::npos) {
      end = str.find(',', start);
      auto usage_ = str.substr(start, end - start);
      if ("output" == usage_) {
        return true;
      }
    }
    return false;
  };

  std::string param_name_type_;
  try {
    for (auto& [param_name_, param_info_] : model_info_.params) {
      // 是否为输出参数
      if (!is_output_param_(param_info_.usage)) {
        continue;
      }
      param_name_type_ = param_name_ + "(" + param_info_.type + ")";

      // 从output_params_中查找数据
      if (auto it = output_params_->find(param_name_); 
          it != output_params_->end()) {
        std::cout << "\t\t" << param_name_type_ << " = "
                  << Print(param_info_.type, it->second, type_define_info_)
                  << std::endl;
      }
    }
  } catch (const std::bad_any_cast &e) {
    std::cerr << "参数" << param_name_type_
              << ") 实际输出数据的类型与模型描述文件中的定义不一致: "
              << e.what() << std::endl;
    return false;
  } catch (const std::exception &e) {
    std::cerr << "模型输出参数中存在类型不一致的错误: " << e.what() << std::endl;
    return false;
  }
  return true;
}
