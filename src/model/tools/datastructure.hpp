#pragma once

#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <array>
#include <unordered_map>
#include <any>
#include "vector3.hpp"
#include "coordinate.hpp"
#include "../framework/componentmanager.hpp"

namespace carPhyModel{

struct Square{
    constexpr static const char* token_list[] = {"length", "width", "height"};
    constexpr operator Vector3() const{
        return Vector3{length, width, height};
    };
    constexpr double operator[](size_t i) const {return (&length)[i];};
    double& operator[](size_t i){return (&length)[i];};
    double length;
    double width;
    double height;
};

// carprotection
struct ProtectionModel{
    constexpr static const char* token_list[] = {"armor", "activeProtectionAmmo", "reactiveArmor", "protectZone"};
    double armor;
    int activeProtectionAmmo;
    int reactiveArmor;
    Vector3 protectZone;
};

// carhull
struct Hull{
    Vector3 velocity;
    Vector3 palstance;
    // double mass;
    // Vector3 I;
};

// cardamage，越小越正常
enum class DAMAGE_LEVEL{
    N, //正常
    M, //中度毁伤
    K, //失去功能
    KK, //失去结构
};

struct PartDamageModel{
    constexpr static const char* token_list[] = {"damageLevel", "size"};
    DAMAGE_LEVEL damageLevel;
    Square size;
};

struct FireEvent{
    std::string weaponName;
    // 命中点的世界坐标
    Vector3 position;
    // 射击方向（开火方指向被命中方）的世界坐标
    Vector3 direction;
    // 终点速度矢量的世界坐标
    Vector3 velocity;
    // 发射者和命中点的直线距离
    double range;
};

// carfireunit
enum class FIRE_UNIT_STATE{
    LOCK,
    LOCK_INVALID,
    FREE,
    SINGLE_SHOOT,
    ALWAYS_SHOOT,
    DESTORY,
};

struct Weapon{
    constexpr static const char* token_list[] = {"ammoType", "ammoRemain", "reloadingTime", "reloadingState"};
    std::string ammoType;
    int ammoRemain;
    double reloadingTime;
    double reloadingState;
};

struct AngleZone{
    constexpr static const char* token_list[] = {"yawLeft", "yawRight", "pitchUp", "pitchDown"};
    double yawLeft, yawRight, pitchUp, pitchDown;
    double& operator[](size_t index){return (&yawLeft)[index];};
    const double& operator[](size_t index) const {return (&yawLeft)[index];};
};

struct Direction{
    constexpr static const char* token_list[] = {"yaw", "pitch"};
    double yaw, pitch;
    double& operator[](size_t index){return (&yaw)[index];};
    const double& operator[](size_t index) const {return (&yaw)[index];};
};

struct FireUnit{
    constexpr static const char* token_list[] = {"state", "fireZone", "rotateZone", "presentDirection", "rotateSpeed", "weapon"};
    FIRE_UNIT_STATE state;
    AngleZone fireZone; //[yawLeft, yawRight, pitchUp, pitchDown]
    AngleZone rotateZone; //[yawLeft, yawRight, pitchUp, pitchDown]
    Direction presentDirection; //[yaw, pitch]
    Direction rotateSpeed; //[yaw, pitch]
    Weapon weapon;
};

// carsensor
struct SensorData{
    constexpr static const char* token_list[] = {"type"};
    std::string type;
};

// vehicle ID
using VID = size_t;

struct EntityInfo{
    Vector3 position;
    Vector3 velocity;
    enum class ENTITY_TYPE{
        CAR,
        UNKNOWN,
    } type;
    VID id;
    double lastScanned;
};

// component ID
using CID = size_t;

struct ScannedMemory : public std::map<VID, EntityInfo>{};

struct WheelMotionParamList{
    constexpr static const char* token_list[] = {
        "radius",
        "LENGTH",
        "MAX_ANGLE",
        "ROTATE_SPEED",
        "MAX_LINEAR_SPEED",
        "MAX_FRONT_ACCELERATION",
        "MAX_BRAKE_ACCELERATION",
        "MAX_LATERAL_ACCELERATION"
    };
    // 车轮转角，右为正
    double radius;
    // 前后轴距
    double LENGTH;
    // 前轮最大转角约束
    double MAX_ANGLE;
    // 车轮转动速度约束
    double ROTATE_SPEED;
    // 最大直线速度约束
    double MAX_LINEAR_SPEED;
    // 最大前向加速度约束
    double MAX_FRONT_ACCELERATION;
    // 最大减速加速度约束
    double MAX_BRAKE_ACCELERATION;
    // 最大转弯向心加速度(侧向加速度)约束
    double MAX_LATERAL_ACCELERATION;
};

struct FireEventQueue : public std::vector<FireEvent>{};

struct OutputBuffer : public std::unordered_map<std::string, std::any>{};

using Components = ComponentManager<
    SingletonComponent<
        OutputBuffer,
        Coordinate,
        FireEventQueue,
        WheelMotionParamList,
        ScannedMemory,
        Hull
    >, NormalComponent<
        Coordinate,
        ProtectionModel,
        PartDamageModel,
        FireUnit,
        SensorData
    >>;

};