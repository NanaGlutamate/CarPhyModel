#include "CarPhyModel.h"
#include "src/model/carbuilder.h"
#include "src/model/tools/constant.hpp"

namespace {

using namespace std;
using namespace carphymodel::command;

constexpr inline double rate = 111000.;

carphymodel::Vector3 locationTrans(const CarPhyModel::Location &base,
                                   const CarPhyModel::Location &location) {
    return {
        rate * (location.latitude - base.latitude),
        rate * (location.longitude - base.longitude) *
            cos(carphymodel::DEG2RAD(base.latitude)),
        base.altitude - location.altitude,
    };
}
CarPhyModel::Location positionTrans(const CarPhyModel::Location &base,
                                    const carphymodel::Vector3 &position) {
    return {
        .longitude =
            position.y / (rate * cos(carphymodel::DEG2RAD(base.latitude))) + base.longitude,
        .latitude = position.x / rate + base.latitude,
        .altitude = base.altitude - position.z,
    };
}

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
    if (auto it = value.find("baseLongitude"); it != value.end()) {
        location.longitude = std::any_cast<double>(it->second);
        location.latitude = std::any_cast<double>(value.find("baseLatitude")->second);
        location.altitude = std::any_cast<double>(value.find("baseAltitude")->second);
    }
    Location tmp{0, 0, 0};
    if (auto it = value.find("longitude"); it != value.end()) {
        tmp.longitude = std::any_cast<double>(it->second);
        tmp.latitude = std::any_cast<double>(value.find("latitude")->second);
        tmp.altitude = std::any_cast<double>(value.find("altitude")->second);
    }
    model.components.getSpecificSingleton<carphymodel::Coordinate>().value().position =
        locationTrans(location, tmp);
    state_ = CSInstanceState::IS_INITIALIZED;
    WriteLog("CarPhyModel model Init", 1);
    return true;
}

bool CarPhyModel::Tick(double time) {
    // TODO: time的单位?
    model.tick(time);
    auto &buffer = model.components.getSpecificSingleton<carphymodel::EventBuffer>();
    buffer->emplace("ForceSideID", GetForceSideID());
    buffer->emplace("ModelID", GetModelID());
    buffer->emplace("InstanceName", GetInstanceName());
    buffer->emplace("ID", GetID());
    buffer->emplace("State", uint16_t(GetState()));
    if (auto it = buffer->find("FireDataOut"); it != buffer->end()) {
        FireEvent tmp = any_cast<carphymodel::FireEvent>(it->second);
        it->second = tmp.ToValueMap();
    }
    carphymodel::Vector3 tmp =
        model.components.getSpecificSingleton<carphymodel::Coordinate>().value().position;
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
    Location tmpl = positionTrans(
        location,
        model.components.getSpecificSingleton<carphymodel::Coordinate>().value().position);
    // deg
    buffer->emplace("longitude", tmpl.longitude);
    buffer->emplace("altitude", tmpl.altitude);
    buffer->emplace("latitude", tmpl.latitude);
    auto attitude =
        carphymodel::Quaternion::fromCompressedQuaternion(
            model.components.getSpecificSingleton<carphymodel::Coordinate>().value().attitude)
            .getEuler();
    // deg
    buffer->emplace("roll", carphymodel::RAD2DEG(attitude.x));
    buffer->emplace("pitch", carphymodel::RAD2DEG(attitude.y));
    buffer->emplace("yaw", carphymodel::RAD2DEG(attitude.z));
    auto velocity = model.components.getSpecificSingleton<carphymodel::Hull>().value().velocity;
    buffer->emplace("velocity_x", velocity.x);
    buffer->emplace("velocity_y", velocity.y);
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
            auto &buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>();
            double param1 = 0., param2 = 0.;
            if (auto it = value.find("Param1"); it != value.end()) {
                param1 = std::any_cast<double>(it->second);
            }
            if (auto it = value.find("Param2"); it != value.end()) {
                param2 = std::any_cast<double>(it->second);
            }
            buffer->emplace(command, make_tuple(param1, param2));
            // TODO:
        }
    }
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
