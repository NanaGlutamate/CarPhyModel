#include "CarPhyModel.h"
#include "src/model/carbuilder.h"

namespace {

using namespace std;

string getLibDir() {
    string library_dir_;
#ifdef _WIN32
    HMODULE module_instance = _AtlBaseModule.GetModuleInstance();
    char dll_path[MAX_PATH] = {0};
    GetModuleFileNameA(module_instance, dll_path, _countof(dll_path));
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    _splitpath_s(dll_path, drive, dir, fname, ext);
    library_dir_ = drive + std::string(dir) + "\\";
#else
    Dl_info dl_info;
    CSModelObject *(*p)() = &CreateModelObject;
    if (0 != dladdr((void *)(p), &dl_info)) {
        library_dir_ = std::string(dl_info.dli_fname);
        library_dir_ = library_dir_.substr(0, library_dir_.find_last_of('/'));
        library_dir_ += "/";
    }
#endif
    return library_dir_;
}

enum class COMMAND_TYPE{
    FORWARD,
    ACCELERATE,
    DECELERATE,
    BACKWARD,
    STOP,
    TURN,
    ACCELERATE_TURN,
    DECELERATE_TURN,
    BACK_TURN,
    SHOOT,
    FREE_SHOOT,
    HOLD_SHOOT,
    AIM,
    LOCK,
    UNLOCK,
    RADAR_SWITCH,
};

} // namespace

bool CarPhyModel::Init(const std::unordered_map<std::string, std::any> &value) {
    if (auto it = value.find("filePath"); it != value.end()) {
        carphymodel::CarBuilder::buildFromFile(any_cast<string>(it->second), model);
    } else {
        carphymodel::CarBuilder::buildFromFile(getLibDir() + "car.xml", model);
    }
    state_ = CSInstanceState::IS_INITIALIZED;
    WriteLog("CarPhyModel model Init", 1);
    return true;
}

bool CarPhyModel::Tick(double time) {
    // TODO: time的单位?
    model.tick(time);

    // 此处填写模型单步运算逻辑
    // 需输出的参数应通过emplace方法写入params_
    // 等候GetOutput接口被调用时参数对外部输出

    // 以下为参数输出示例
    //
    // 输出参数EntityInfoOut
    // params_.emplace("EntityInfoOut", EntityInfoOut_.ToValueMap());
    //
    // 输出参数FireDataOut
    // params_.emplace("FireDataOut", FireDataOut_.ToValueMap());

    WriteLog("CarPhyModel model Tick", 1);
    return true;
}

bool CarPhyModel::SetInput(
    const std::unordered_map<std::string, std::any> &value) {
    auto ID = any_cast<carphymodel::VID>(value.find("ID")->second);
    for (auto &&[k, v] : value) {
        if (k == "EntityInfo") {
            EntityInfo tmp;
            tmp.FromValueMap(any_cast<CSValueMap>(v));
            get<1>((*(model.components.getSpecificSingletonComponent<carphymodel::ScannedMemory>()))[ID]) = tmp;
        }else if(k == "FireData"){
            FireEvent tmp;
            tmp.FromValueMap(any_cast<CSValueMap>(v));
            model.components.getSpecificSingletonComponent<carphymodel::FireEventQueue>()->push_back(tmp);
        }else if(k == "Command"){
            auto Command = std::any_cast<uint64_t>(v);
            vector<double> Params;
            auto t_Params = std::any_cast<std::vector<std::any>>(value.find("Params")->second);
            for (auto p : t_Params) {
                auto pp = std::any_cast<double>(p);
                Params.push_back(pp);
            }
            //TODO:
        }
    }
    // if (auto it = value.find("EntityInfo"); it != value.end()) {
    //     auto t_EntityInfo = std::any_cast<CSValueMap>(it->second);
    //     EntityInfo_.FromValueMap(t_EntityInfo);
    // }
    // if (auto it = value.find("FireData"); it != value.end()) {
    //     auto t_FireData = std::any_cast<CSValueMap>(it->second);
    //     FireData_.FromValueMap(t_FireData);
    // }
    // if (auto it = value.find("Command"); it != value.end()) {
    //     Command = std::any_cast<uint64_t>(it->second);
    // }
    // if (auto it = value.find("Params"); it != value.end()) {
    //     auto t_Params = std::any_cast<std::vector<std::any>>(it->second);
    //     for (auto p : t_Params) {
    //         auto pp = std::any_cast<double>(p);
    //         Params_.push_back(pp);
    //     }
    // }

    WriteLog("CarPhyModel model SetInput", 1);
    return true;
}

std::unordered_map<std::string, std::any> *CarPhyModel::GetOutput() {
    state_ = CSInstanceState::IS_RUNNING;
    params_.emplace("ForceSideID", GetForceSideID());
    params_.emplace("ModelID", GetModelID());
    params_.emplace("InstanceName", GetInstanceName());
    params_.emplace("ID", GetID());
    params_.emplace("State", uint16_t(GetState()));

    WriteLog("CarPhyModel model GetOutput", 1);
    return &params_;
}

CARPHYMODEL_EXPORT CSModelObject *CreateModelObject() {
    CSModelObject *model = new CarPhyModel();
    return model;
}

CARPHYMODEL_EXPORT void DestroyMemory(void *mem, bool is_array) {
    if (is_array) {
        delete[] ((CarPhyModel *)mem);
    } else {
        delete ((CarPhyModel *)mem);
    }
}
