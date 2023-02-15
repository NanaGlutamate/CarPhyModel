#include "CarPhyModel.h"
#include "src/model/carbuilder.h"

namespace {

using namespace std;
using namespace carphymodel::command;

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
    auto &buffer = model.components.getSpecificSingleton<carphymodel::OutputBuffer>();
    buffer->emplace("ForceSideID", GetForceSideID());
    buffer->emplace("ModelID", GetModelID());
    buffer->emplace("InstanceName", GetInstanceName());
    buffer->emplace("ID", GetID());
    buffer->emplace("State", uint16_t(GetState()));
    if (auto it = buffer->find("FireDataOut"); it != buffer->end()) {
        FireEvent tmp = any_cast<carphymodel::FireEvent>(it->second);
        it->second = tmp.ToValueMap();
    }
    EntityInfo info{
        .baseInfo =
            BaseInfo{
                .id = GetID(),
                .side = GetForceSideID(),
                .type = static_cast<uint16_t>(carphymodel::BaseInfo::ENTITY_TYPE::CAR),
                .damageLevel = static_cast<uint16_t>(
                    model.components.getSpecificSingleton<carphymodel::DamageModel>()
                        ->damageLevel),
            }, 
        .position =
            model.components.getSpecificSingleton<carphymodel::Coordinate>()->position,
        .velocity = model.components.getSpecificSingleton<carphymodel::Hull>()->velocity,
    };
    buffer->emplace("EntityInfoOut", info.ToValueMap());

    WriteLog("CarPhyModel model Tick", 1);
    return true;
}

bool CarPhyModel::SetInput(const std::unordered_map<std::string, std::any> &value) {
    auto ID = any_cast<carphymodel::VID>(value.find("ID")->second);
    for (auto &&[k, v] : value) {
        if (k == "EntityInfo") {
            EntityInfo tmp;
            tmp.FromValueMap(any_cast<CSValueMap>(v));
            get<1>((
                *(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[ID]) =
                tmp;
        } else if (k == "FireData") {
            FireEvent tmp;
            tmp.FromValueMap(any_cast<CSValueMap>(v));
            model.components.getSpecificSingleton<carphymodel::FireEventQueue>()->push_back(
                tmp);
        } else if (k == "Command") {
            auto command = static_cast<COMMAND_TYPE>(std::any_cast<uint64_t>(v));
            auto &buffer = model.components.getSpecificSingleton<carphymodel::InputBuffer>();
            double param1 = 0., param2 = 0.;
            if (!NoParam.contains(command)) {
                param1 = std::any_cast<double>(value.find("Param1")->second);
            }
            if (DoubleParam.contains(command)) {
                param2 = std::any_cast<double>(value.find("Param2")->second);
            }
            buffer->emplace(command, make_tuple(param1, param2));
            // TODO:
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
    //     command = std::any_cast<uint64_t>(it->second);
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
    WriteLog("CarPhyModel model GetOutput", 1);
    return model.getOutput();
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
