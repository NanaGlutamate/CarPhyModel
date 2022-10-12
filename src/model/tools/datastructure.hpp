#ifndef __SRC_MODEL_PCH_H__
#define __SRC_MODEL_PCH_H__

#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <array>
#include "vector3.hpp"
#include "coordinate.hpp"
#include "../framework/componentmanager.hpp"

// carprotection
struct ProtectionModel{
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

// cardamage，越大越正常
enum class DAMAGE_LEVEL{
    KK, //失去结构
    K, //失去功能
    M, //中度毁伤
    N, //正常
};

struct PartDamageModel{
    DAMAGE_LEVEL damageLevel;
    Vector3 size;
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
    std::string ammoType;
    int ammoRemain;
    double reloadingTime;
    double reloadingState;
};

struct FireUnit{
    FIRE_UNIT_STATE state;
    std::array<double, 4> fireZone; //[yawLeft, yawRight, pitchUp, pitchDown]
    std::array<double, 4> rotateZone; //[yawLeft, yawRight, pitchUp, pitchDown]
    std::array<double, 2> presentDirection; //[yaw, pitch]
    std::array<double, 2> rotateSpeed; //[yaw, pitch]
    Weapon weapon;
};

// carsensor
struct SensorData{
    std::string type;
};

enum class ENTITY_TYPE{
    CAR,
    UNKNOWN,
};

// vehicle ID
using VID = size_t;

struct EntityInfo{
    Vector3 position;
    Vector3 velocity;
    ENTITY_TYPE type;
    VID id;
    double lastScanned;
};

// component ID
using CID = size_t;

// car model component list
// using CoordinateList = std::vector<std::tuple<CID, Coordinate>>;
// using ProtectionList = std::vector<std::tuple<CID, ProtectionModel>>;
// using DamageList = std::vector<std::tuple<CID, PartDamageModel>>;
// using FireUnitList = std::vector<std::tuple<CID, FireUnit>>;
// using SensorList = std::vector<std::tuple<CID, SensorData>>;
using ScannedMemory = std::map<VID, EntityInfo>;

struct WheelMotionParamList{
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

using Components = ComponentManager<
    SingletonComponent<
        WheelMotionParamList,
        ScannedMemory,
        Hull,
    >, NormalComponent<
        Coordinate,
        ProtectionModel,
        PartDamageModel,
        FireUnit,
        SensorData,
    >>;

#endif