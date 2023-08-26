#pragma once

#include "src/model/tools/datastructure.hpp"
#include "../csmodel_base/model_struct.h"

// BaseInfo
// BaseInfo
class BaseInfo {
  public:
    CSValueMap ToValueMap() {
        CSValueMap value;
        value.emplace("id", id);
        value.emplace("side", side);
        value.emplace("type", type);
        value.emplace("damageLevel", damageLevel);
        return value;
    }

    void FromValueMap(const CSValueMap &value) {
        for (auto &it : value) {
            if (it.first == "id")
                id = std::any_cast<uint64_t>(it.second);
            if (it.first == "side")
                side = std::any_cast<uint16_t>(it.second);
            if (it.first == "type")
                type = std::any_cast<uint16_t>(it.second);
            if (it.first == "damageLevel")
                damageLevel = std::any_cast<uint16_t>(it.second);
        }
    }

    operator carphymodel::BaseInfo() const {
        return carphymodel::BaseInfo{
            .type = static_cast<carphymodel::BaseInfo::ENTITY_TYPE>(type),
            .id = id,
            .side = side,
            .damageLevel = static_cast<carphymodel::DAMAGE_LEVEL>(damageLevel),
        };
    }

  public:
    // id
    uint64_t id;
    // side
    uint16_t side;
    // type
    uint16_t type;
    uint16_t damageLevel;
};

// Vector3
// Vector3
class Vector3 {
  public:
    CSValueMap ToValueMap() {
        CSValueMap value;
        value.emplace("x", x);
        value.emplace("y", y);
        value.emplace("z", z);
        return value;
    }

    Vector3() = default;

    void FromValueMap(const CSValueMap &value) {
        for (auto &it : value) {
            if (it.first == "x")
                x = std::any_cast<double>(it.second);
            if (it.first == "y")
                y = std::any_cast<double>(it.second);
            if (it.first == "z")
                z = std::any_cast<double>(it.second);
        }
    }

    Vector3(const carphymodel::Vector3& v){
        x = v.x;
        y = v.y;
        z = v.z;
    }

    operator carphymodel::Vector3() const { return carphymodel::Vector3(x, y, z); }

  public:
    // x
    double x;
    // y
    double y;
    // z
    double z;
};

// EntityInfo
// EntityInfo
class EntityInfo {
  public:
    CSValueMap ToValueMap() {
        CSValueMap value;
        value.emplace("baseInfo", baseInfo.ToValueMap());
        value.emplace("position", position.ToValueMap());
        value.emplace("velocity", velocity.ToValueMap());
        return value;
    }

    void FromValueMap(const CSValueMap &value) {
        for (auto &it : value) {
            if (it.first == "baseInfo") {
                auto temp = std::any_cast<CSValueMap>(it.second);
                baseInfo.FromValueMap(temp);
            }
            if (it.first == "position") {
                auto temp = std::any_cast<CSValueMap>(it.second);
                position.FromValueMap(temp);
            }
            if (it.first == "velocity") {
                auto temp = std::any_cast<CSValueMap>(it.second);
                velocity.FromValueMap(temp);
            }
        }
    }

    operator carphymodel::EntityInfo() const {
        return carphymodel::EntityInfo{
            .position = position, 
            .velocity = velocity,
            .baseInfo = baseInfo,
        };
    }

    EntityInfo() = default;

    EntityInfo(const carphymodel::EntityInfo& e) {
        baseInfo.damageLevel = static_cast<uint16_t>(e.baseInfo.damageLevel);
        baseInfo.id = e.baseInfo.id;
        baseInfo.side = e.baseInfo.side;
        baseInfo.type = static_cast<uint16_t>(e.baseInfo.type);
        position = e.position;
        velocity = e.velocity;
    }

  public:
    // baseInfo
    BaseInfo baseInfo;
    // position
    Vector3 position;
    // velocity
    Vector3 velocity;
};

// FireEvent
// FireEvent
class FireEvent {
  public:
    CSValueMap ToValueMap() {
        CSValueMap value;
        value.emplace("position", position.ToValueMap());
        value.emplace("range", range);
        value.emplace("target", target.ToValueMap());
        value.emplace("velocity", velocity.ToValueMap());
        value.emplace("weaponName", weaponName);
        return value;
    }

    void FromValueMap(const CSValueMap &value) {
        for (auto &it : value) {
            if (it.first == "position") {
                auto temp = std::any_cast<CSValueMap>(it.second);
                position.FromValueMap(temp);
            }
            if (it.first == "range")
                range = std::any_cast<double>(it.second);
            if (it.first == "target") {
                auto temp = std::any_cast<CSValueMap>(it.second);
                target.FromValueMap(temp);
            }
            if (it.first == "velocity") {
                auto temp = std::any_cast<CSValueMap>(it.second);
                velocity.FromValueMap(temp);
            }
            if (it.first == "weaponName")
                weaponName = std::any_cast<std::string>(it.second);
        }
    }

    FireEvent() = default;    

    FireEvent(const carphymodel::FireEvent& f) {
        weaponName = f.weaponName;
        target = f.target;
        position = f.position;
        velocity = f.velocity;
        range = f.range;
    }

    operator carphymodel::FireEvent () const {
        return carphymodel::FireEvent{
            .weaponName = weaponName,
            .target = target,
            .position = position,
            .velocity = velocity,
            .range = range,
        };
    }

  public:
    // position
    Vector3 position;
    // range
    double range;
    // target
    Vector3 target;
    // velocity
    Vector3 velocity;
    // weaponName
    std::string weaponName;
};

// WeaponInfo
struct WeaponInfo {
    int32_t ammo;
    double reloadingState;
    double range;
    // yaw, deg
    double direction;
    std::string weaponName;
    CSValueMap ToValueMap() const {
        CSValueMap value;
        value.emplace("ammo", ammo);
        value.emplace("reloadingState", reloadingState);
        value.emplace("range", range);
        value.emplace("direction", direction);
        value.emplace("weaponName", weaponName);
        return value;
    }
    void FromValueMap(const CSValueMap &value) {
        for (auto &it : value) {
            if (it.first == "ammo")
                ammo = std::any_cast<int>(it.second);
            if (it.first == "reloadingState")
                reloadingState = std::any_cast<double>(it.second);
            if (it.first == "range")
                range = std::any_cast<double>(it.second);
            if (it.first == "direction")
                direction = std::any_cast<double>(it.second);
            if (it.first == "weaponName")
                weaponName = std::any_cast<std::string>(it.second);
        }
    }
};
