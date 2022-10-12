//#pragma once

// "csmodel_base.h"
//#include "common_struct.h"
//#include "radar66_export.h"

// Radar66
// 
//class Radar66{
// public:
//  // 初始化
//  virtual bool Init(const std::unordered_map<std::string, std::any> &value);
//  // 单步运算 time 应推进的步长(ms)
//  virtual bool Tick(double time);
//  // 获取输入参数
//  virtual bool SetInput(const std::unordered_map<std::string, std::any>& value);
//  // 对外部输出模型参数
//  virtual std::unordered_map<std::string, std::any>* GetOutput();
//
// private:
//  // JammerVel 干扰机速度 
//  V3f JammerVel_;
//  // JammerPos 干扰机位置 
//  V3f JammerPos_;
//  // RadarPos 雷达位置 
//  V3f RadarPos_;
//  // RadarVel 雷达速度 
//  V3f RadarVel_;
//  // TargetPos 目标位置 
//  V3f TargetPos_;
//  // TargetVel 目标速度 
//  V3f TargetVel_;
//  // 所属阵营  
//  uint16_t ForceSideID_;
//  // 关键信息  
//  std::string KeyMessages_;
//  // uint64  
//  uint64_t ID_;
//  // 名称  
//  std::string InstanceName_;
//  // JammerFreRange 干扰机频率范围 
//  double JammerFreRange_;
//  // JammerFre 干扰机频率 
//  double JammerFre_;
//  // TargetRCS RCS 
//  double TargetRCS_;
//  // 模型ID  
//  std::string ModelID_;
//  // RadarBand 雷达带宽 
//  double RadarBand_;
//  // RadarCenter 雷达中心频率 
//  double RadarCenter_;
//  // detect_result 探测结果 
//  std::string detect_result_;
//  // 模型状态  
//  uint16_t State_;
//};
