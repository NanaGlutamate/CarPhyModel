#include <format>
#include <string>
#include <chrono>

#include "CarPhyModel.h"
#include "src/model/carbuilder.h"
#include "src/model/tools/constant.hpp"
#include "src/model/tools/rand.hpp"
#include "src/model/environment/wsf.hpp"

namespace {

using namespace std;
using namespace carphymodel::command;

constexpr inline double rate = 111000.;

carphymodel::Vector3 locationTrans(const CarPhyModel::Location& base, const CarPhyModel::Location& location) {
    return {
        rate * (location.latitude - base.latitude),
        rate * (location.longitude - base.longitude) * cos(carphymodel::DEG2RAD(base.latitude)),
        base.altitude - location.altitude,
    };
}
CarPhyModel::Location positionTrans(const CarPhyModel::Location& base, const carphymodel::Vector3& position) {
    return {
        .longitude = position.y / (rate * cos(carphymodel::DEG2RAD(base.latitude))) + base.longitude,
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
    CSModelObject* (*p)() = &CreateModelObject;
    if (0 != dladdr((void*)(p), &dl_info)) {
        library_dir_ = std::string(dl_info.dli_fname);
        library_dir_ = library_dir_.substr(0, library_dir_.find_last_of('/'));
        library_dir_ += "/";
    }
#endif
    return library_dir_;
}

} // namespace

bool CarPhyModel::Init(const std::unordered_map<std::string, std::any>& value) {
    if (auto it = value.find("filePath"); it != value.end()) {
        carphymodel::CarBuilder::buildFromFile(any_cast<std::string>(it->second), model);
    } else {
        carphymodel::CarBuilder::buildFromFile(getLibDir() + "car.xml", model);
    }
    Location tmp{0, 0, 0};
    tmp.longitude = std::any_cast<double>(value.find("longitude")->second);
    tmp.latitude = std::any_cast<double>(value.find("latitude")->second);
    tmp.altitude = std::any_cast<double>(value.find("altitude")->second);
    {
        std::lock_guard<std::mutex> lock(initLock);
        myVID = VIDCounter++;
        if (!myVID) {
            // location of car 0 is base location.
            // CQ will not release dll when restart, but has no unexpected affect
            location = tmp;
            if (auto it = value.find("demFilePath"); it != value.end()) {
                auto env = std::make_unique<wsfplugin::WSFEnvironment>();
                auto filePath = any_cast<std::string>(it->second);
                env->init(filePath, tmp.longitude, tmp.latitude);
                carphymodel::EnvironmentInfoAgent::changeEnvironmentSupplier(std::move(env));
            }
        }
    }
    auto& buffer = model.components.getSpecificSingleton<carphymodel::EventBuffer>().value();
    buffer.emplace("longitude", tmp.longitude);
    buffer.emplace("latitude", tmp.latitude);
    model.components.getSpecificSingleton<carphymodel::Coordinate>().value().position = locationTrans(location, tmp);
    state_ = CSInstanceState::IS_RUNNING;
    lastFrameTime = std::chrono::high_resolution_clock::now();
    return true;
}

bool CarPhyModel::Tick(double time) {
    //first deal with Systemscannedmemoryget to Systemscannedmemory
    size_t memsize = model.components.getSpecificSingleton<carphymodel::SystemScannedMemoryget>().value().size();//所有车的扫描实体列表，vector<vector<EntityInfo>>
    auto& memit = model.components.getSpecificSingleton<carphymodel::ScannedMemory>();
    auto& memsysscan = model.components.getSpecificSingleton<carphymodel::SystemScannedMemory>();
    for (auto& infomem : model.components.getSpecificSingleton<carphymodel::SystemScannedMemoryget>().value())
    {
        if (memit.value().find(infomem.first) != memit.value().end() && get<1>((*(memit))[infomem.first]).baseInfo.side == GetForceSideID())
        {//遍历infomem，更新memsysscan
            for (auto& info : get<1>(infomem.second))
            {
                //判断扫描到的车在不在扫描车的探测范围内___________________________________________________
                if ((info.second.position - get<1>((*(memit))[infomem.first]).position).norm() < 10000) {
                    carphymodel::VID IDmem = info.second.baseInfo.id;
                    get<1>((*(model.components.getSpecificSingleton<carphymodel::SystemScannedMemory>()))[IDmem]) =
                        info.second;
                }
            }
        }
    }
    // time: ms -> s
    model.tick(time / 1000);
    //声明一个变量记录当前的系统时间
    auto currentFrameTime = std::chrono::high_resolution_clock::now();
    // 计算时间差
    std::chrono::duration<double> deltaTime = currentFrameTime - lastFrameTime;
    // 更新上一帧时间
    lastFrameTime = currentFrameTime;
    double dtmessage = deltaTime.count();

    timeiter += time;
    auto& buffer = model.components.getSpecificSingleton<carphymodel::EventBuffer>();
    buffer->emplace("VID", getVID());
    if (auto it = buffer->find("FireDataOut"); it != buffer->end()) {
        FireEvent tmp = any_cast<carphymodel::FireEvent>(it->second);
        if (carphymodel::testRandom(0.05/*0.9*/)) {
            buffer->erase(it);//fire_unsucceed,
        } else {
            WriteLog(format("carphymodel send fireEvent: {{weapon: {}, from: {}({}, {}, {}), to: ({}, {}, {})}}",
                            tmp.weaponName, getVID(), tmp.position.x, tmp.position.y, tmp.position.z, tmp.target.x,
                            tmp.target.y, tmp.target.z));
            it->second = tmp.ToValueMap();
        }
    }
    carphymodel::Vector3 tmp = model.components.getSpecificSingleton<carphymodel::Coordinate>().value().position;
    EntityInfo info;
    info.baseInfo = BaseInfo{
        getVID(),
        GetForceSideID(),
        static_cast<uint16_t>(carphymodel::BaseInfo::ENTITY_TYPE::CAR),
        static_cast<uint16_t>(model.components.getSpecificSingleton<carphymodel::DamageModel>()->damageLevel),
        get<1>(*model.components.getNormal<carphymodel::ProtectionModel>().begin()).jammer,//约定第一个entity反映整体特征
        get<1>(*model.components.getNormal<carphymodel::ProtectionModel>().begin()).hidden,//约定第一个entity反映整体特征
        get<1>(*model.components.getNormal<carphymodel::ProtectionModel>().begin()).active_interference_rate,//约定第一个entity反映整体特征
        get<1>(*model.components.getNormal<carphymodel::ProtectionModel>().begin()).active_interference_distance,//约定第一个entity反映整体特征
    };
    if (info.baseInfo.damageLevel >= static_cast<uint16_t>(carphymodel::DAMAGE_LEVEL::K)) {
        state_ = CSInstanceState::IS_DESTROYED;
    }
    info.position = model.components.getSpecificSingleton<carphymodel::Coordinate>()->position;
    info.velocity = model.components.getSpecificSingleton<carphymodel::Hull>()->velocity;
    
    buffer->emplace("EntityInfoOut", info.ToValueMap());
    Location tmpl = positionTrans(location, info.position);
    double oilremain = model.components.getSpecificSingleton<carphymodel::WheelMotionParamList>()->OIL_REMAIN;
    // deg
    buffer->emplace("longitude", tmpl.longitude);
    buffer->emplace("altitude", tmpl.altitude);
    buffer->emplace("latitude", tmpl.latitude);
    auto attitude = carphymodel::Quaternion::fromCompressedQuaternion(
                        model.components.getSpecificSingleton<carphymodel::Coordinate>().value().attitude)
                        .getEuler();
    // deg
    buffer->emplace("roll", carphymodel::RAD2DEG(attitude.x));
    buffer->emplace("pitch", carphymodel::RAD2DEG(attitude.y));
    buffer->emplace("yaw", carphymodel::RAD2DEG(attitude.z));
    auto velocity = model.components.getSpecificSingleton<carphymodel::Hull>().value().velocity;
    buffer->emplace("velocity_x", velocity.x);
    buffer->emplace("velocity_y", velocity.y);
    buffer->emplace("oil_remain", oilremain);
    buffer->emplace("weapon1_ammoremain",
        static_cast<double>(get<1>(*model.components.getNormal<carphymodel::FireUnit>().begin()).weapon.ammoRemain));
    buffer->emplace("target_id", get<1>(*model.components.getNormal<carphymodel::FireUnit>().begin()).data);

    std::vector<std::any> weaponInfo;
    auto& baseCoordinate = model.components.getSpecificSingleton<carphymodel::Coordinate>().value();
    for (auto&& [id, info, coordinate] : model.components.getNormal<carphymodel::FireUnit, carphymodel::Coordinate>()) {
        carphymodel::Vector3 relativeDirection = {cos(info.presentDirection.yaw), sin(info.presentDirection.yaw), 0};
        auto tmp = coordinate.directionBodyToWorld(relativeDirection);
        auto globalDirection = baseCoordinate.directionBodyToWorld(tmp);
        weaponInfo.emplace_back(WeaponInfo{(int32_t)info.weapon.ammoRemain, info.weapon.reloadingState,
                                           info.weapon.range, atan2(globalDirection.y, globalDirection.x),
                                           info.weapon.ammoType}
                                    .ToValueMap());
    }
    buffer->emplace("weaponInfoOut", std::move(weaponInfo));

    std::vector<std::any> scannedInfoOut;
    for (auto& info : model.components.getSpecificSingleton<carphymodel::ScannedMemory>().value()) {
        //判断info中的tuple中的double参数是否==0
        int i = 0;
        if (std::get<0>(info.second) == 0.)
        {
            scannedInfoOut.emplace_back(EntityInfo(std::get<1>(info.second)).ToValueMap());
            //scannedInfoOut.emplace_back(EntityInfo(std::get<1>(info.second)).ToValueMap());
            carphymodel::VID lcc1 = getVID();
            std::cout << "lcc1:" << lcc1 << std::endl;
            i++;
            if (lcc1 == uint64_t(1) && i == 2)
            std::cout << lcc1 << std::endl;
        WriteLog(std::format("test0905 当前车辆ID{};目标ID为{},", getVID(), std::get<1>(info.second).baseInfo.id));
        }
        std::cout << "tance:" << i << std::endl;
    }
    WriteLog(std::format("test0905 当前车辆ID{};扫描到的实体列表大小为{},", getVID(), scannedInfoOut.size()));
    buffer->emplace("scannedInfoOut", std::move(scannedInfoOut));
    //ask for if a param can be used for dds and send to behaviour model simutaneously;
    //std::vector<std::any> scannedInfoOut1;
    //for (auto& info : model.components.getSpecificSingleton<carphymodel::ScannedMemory>().value()) {
    //    // 判断info中的tuple中的double参数是否==0
    //    if (std::get<0>(info.second) == 0.) {
    //        // scannedInfoOut.emplace_back(EntityInfo(std::get<1>(info.second)).ToValueMap());
    //    }
    //    scannedInfoOut1.emplace_back(EntityInfo(std::get<1>(info.second)).ToValueMap());
    //}
    //buffer->emplace("scannedInfoOut1", std::move(scannedInfoOut1));
    //auto it = buffer->find("FireDataOut");
    if (getVID() == 0 /*|| (static_cast<int>(timeiter) % 5000 == 0 ||it != buffer->end())*/ ) {
        std::vector<std::any> messages;
        int i = 1;
        auto& it = model.components.getSpecificSingleton<carphymodel::SystemScannedMemory>().value();
        for (auto& info : model.components.getSpecificSingleton<carphymodel::ScannedMemory>().value()) {
            // 判断info中的tuple中的double参数是否==0
            auto targetID = std::get<1>(info.second).baseInfo.id + 1;
            if (std::get<0>(info.second) == 0. && std::get<1>(info.second).baseInfo.side != GetForceSideID()) {
                // scannedInfoOut.emplace_back(EntityInfo(std::get<1>(info.second)).ToValueMap());
                messages.push_back(std::format("当前车辆所在编队探测到的第{}个,目标ID为{}", (uint64_t)i, targetID));
                i += 1;
                continue;
            }
            if (it.find(std::get<1>(info.second).baseInfo.id) != it.end() && std::get<1>(info.second).baseInfo.side != GetForceSideID())
            {
                messages.push_back(std::format("当前车辆未探测到，编队系统中前出侦查车辆探测到的第{}个,目标ID为{}", (uint64_t)i, targetID));
                i += 1;
            }
        }
        if (areVectorsEqual(messages, lasttimemessage))
            messages.clear();
        else
            lasttimemessage = messages;
        if (auto it = buffer->find("FireDataOut"); it != buffer->end()) {
            FireEvent tmp = any_cast<FireEvent>(it->second);
            Location tmp1 = positionTrans(location, tmp.position);
            Location tmp2 = positionTrans(location, tmp.target);
            messages.push_back(/*std::format("carphymodel send fireEvent,range{}m", (uint64_t)tmp.range)*/
                               std::string("fire"));
        }
        //messages.push_back(std::format("相邻两帧运行的真实时间为{}s", deltaTime.count()));
        buffer->emplace("KeyMessages", messages);
    }
    model.components.getSpecificSingleton<carphymodel::SystemScannedMemory>().value().clear();
    model.components.getSpecificSingleton<carphymodel::SystemScannedMemoryget>().value().clear();
    return true;
}

bool CarPhyModel::SetInput(const std::unordered_map<std::string, std::any>& value) {
    if (auto it = value.find("EntityInfo"); it != value.end()) {
        auto& v = it->second;
        EntityInfo tmp;
        tmp.FromValueMap(any_cast<CSValueMap>(v));
        carphymodel::VID ID = tmp.baseInfo.id;
        get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[ID]) = tmp;
        if (auto it = value.find("scannedInfo"); it != value.end()/* && tmp.baseInfo.side == GetForceSideID()*/) {
            auto& v1 = it->second;
            std::vector<std::any> tmp_sc = any_cast<std::vector<std::any>>(v1);
            size_t size_tmp = tmp_sc.size();
            //std::vector<EntityInfo> tmp1;
            //tmp1.reserve(size_tmp);
            //tmp1.resize(static_cast<int>(size_tmp));
            //WriteLog(std::format("test0905 当前车辆ID{};目标ID为{},", getVID(), std::get<1>(info.second).baseInfo.id));
            WriteLog(std::format("test0905 当前接收到的探测信息来源于车辆ID{};该车扫描到的实体列表大小为{}", ID, size_tmp));
            for (size_t i = 0; i < size_tmp; i++) {
                EntityInfo lcc;
                lcc.FromValueMap(any_cast<CSValueMap>(tmp_sc[i]));
                //tmp1[i] = lcc;
                carphymodel::VID NewID = lcc.baseInfo.id;
                get<1>((*(model.components.getSpecificSingleton<carphymodel::SystemScannedMemoryget>()))[ID])[NewID] = lcc;
                WriteLog(std::format("test0905 当前接收到的探测信息来源于车辆ID{};目标ID为{},", getVID(), NewID));
            }
        }
    }
    //if (auto it = value.find("scannedInfo"); it != value.end()) {
    //    auto& v = it->second;
    //    std::vector<std::any> tmp_sc;
    //    std::vector<EntityInfo> tmp1;
    //    if (any_cast<uint16_t>(value.find("forceSideID")->second) == force_side_id_ ) {
    //        tmp_sc = any_cast<std::vector<std::any>>(v);
    //        size_t size_tmp = tmp_sc.size();
    //        tmp1.resize(size_tmp);
    //        for (size_t i = 0; i < size_tmp; i++) {
    //            tmp1[i].FromValueMap(any_cast<CSValueMap>(tmp_sc[i]));
    //            EntityInfo tmp1_look;
    //            tmp1_look = tmp1[i];
    //            carphymodel::VID ID;
    //            ID = tmp1[i].baseInfo.id;
    //            get<1>((*(model.components.getSpecificSingleton<carphymodel::SystemScannedMemory>()))[ID]) = tmp1[i];
    //        }
    //    }
    //    //for debug
    //    //uint64_t de = 4;
    //    //auto debug_it = model.components.getSpecificSingleton<carphymodel::SystemScannedMemory>().value().find(de);
    //    //if (getVID() == 0 && debug_it != model.components.getSpecificSingleton<carphymodel::SystemScannedMemory>().value().end())
    //    //{
    //    //    size_t size_tmp;
    //    //    size_tmp = model.components.getSpecificSingleton<carphymodel::SystemScannedMemory>().value().size();
    //    //    cout << size_tmp << endl;
    //    //    for (size_t i = 0; i < tmp1.size(); i++)
    //    //    {
    //    //        WriteLog(format("wsf{},{},{},tmpsize{};lcccccccc{},{},{},tmpsize{}", getVID(),
    //    //                        any_cast<uint16_t>(value.find("forceSideID")->second), force_side_id_, tmp_sc.size(),
    //    //                        tmp1[i].baseInfo.id, tmp1[i].baseInfo.side, tmp1[i].position.x, tmp1.size()));
    //    //    //WriteLog(format("lcccccccc{},{},{},tmpsize{}", tmp1[i].baseInfo.id, tmp1[i].baseInfo.side, tmp1[i].position.x, tmp1.size()));
    //    //    }
    //    //}
    //}
    if (auto it = value.find("FireData"); it != value.end()) {
        carphymodel::VID ID = any_cast<carphymodel::VID>(value.find("FireID")->second);
        auto& v = it->second;
        if (ID != getVID()) {
            FireEvent tmp;
            tmp.FromValueMap(any_cast<CSValueMap>(v));
            model.components.getSpecificSingleton<carphymodel::FireEventQueue>()->push_back(tmp);
        }
    }
    if (auto it = value.find("Command"); it != value.end()) {
        auto& v = it->second;
        auto command = static_cast<COMMAND_TYPE>(std::any_cast<uint64_t>(v));
        auto& buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>();
        double param1 = 0., param2 = 0.;
        if (auto it = value.find("Param1"); it != value.end()) {
            param1 = std::any_cast<double>(it->second);
        }
        if (auto it = value.find("Param2"); it != value.end()) {
            param2 = std::any_cast<double>(it->second);
        }
        WriteLog(std::format("CarPhyModel model receive command {}({},{})", (uint64_t)command, param1, param2), 1);
        buffer->emplace(command, make_tuple(param1, param2));
    }
    return true;
}

std::unordered_map<std::string, std::any>* CarPhyModel::GetOutput() {
    std::get<0>(model.components.getSingleton<carphymodel::VID>()) = getVID();
    std::get<0>(model.components.getSingleton<carphymodel::SID>()) = GetForceSideID();
    auto& buffer = model.components.getSpecificSingleton<carphymodel::EventBuffer>().value();
    buffer.emplace("ForceSideID", GetForceSideID());
    buffer.emplace("ModelID", GetModelID());
    buffer.emplace("InstanceName", GetInstanceName());
    buffer.emplace("ID", GetID());
    buffer.emplace("State", uint16_t(GetState()));
    /*std::vector<std::any> messages = {"nihao", "nihao1"};
    buffer.emplace("KeyMessages", messages);*/
    //success below
    /*std::vector<std::any> messages = {std::string("nihao"), std::string("nihao1")};
    buffer["KeyMessages"]=messages;*/
    return &buffer;
}

CARPHYMODEL_EXPORT CSModelObject* CreateModelObject() {
    CSModelObject* model = new CarPhyModel();
    return model;
}

CARPHYMODEL_EXPORT void DestroyMemory(void* mem, bool is_array) {
    if (is_array) {
        delete[] ((CarPhyModel*)mem);
    } else {
        delete ((CarPhyModel*)mem);
    }
}
