#pragma once

#include <any>
#include <array>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../framework/componentmanager.hpp"
#include "coordinate.hpp"
#include "constant.hpp"
#include "vector3.hpp"

namespace carphymodel {

namespace command {

enum class COMMAND_TYPE {
    FORWARD = 1,
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
    STOP_SHOOT,
    LOCK_DIRECTION,
    LOCK_TARGET,
    UNLOCK,
    RADAR_SWITCH,
    FOLLOW_ROAD,
    SET_ROAD,
};

inline size_t NoParamMask =
    size_t(1) << static_cast<int>(COMMAND_TYPE::FORWARD) | size_t(1) << static_cast<int>(COMMAND_TYPE::STOP) |
    size_t(1) << static_cast<int>(COMMAND_TYPE::FREE_SHOOT) | size_t(1) << static_cast<int>(COMMAND_TYPE::STOP_SHOOT);

inline size_t SingleParamMask =
    size_t(1) << static_cast<int>(COMMAND_TYPE::ACCELERATE) | size_t(1) << static_cast<int>(COMMAND_TYPE::DECELERATE) |
    size_t(1) << static_cast<int>(COMMAND_TYPE::BACKWARD) | size_t(1) << static_cast<int>(COMMAND_TYPE::TURN) |
    size_t(1) << static_cast<int>(COMMAND_TYPE::FOLLOW_ROAD) | size_t(1) << static_cast<int>(COMMAND_TYPE::UNLOCK);

inline size_t DoubleParamMask = size_t(1) << static_cast<int>(COMMAND_TYPE::ACCELERATE_TURN) |
                                size_t(1) << static_cast<int>(COMMAND_TYPE::DECELERATE_TURN) |
                                size_t(1) << static_cast<int>(COMMAND_TYPE::BACK_TURN) |
                                size_t(1) << static_cast<int>(COMMAND_TYPE::SHOOT) |
                                size_t(1) << static_cast<int>(COMMAND_TYPE::LOCK_DIRECTION) |
                                size_t(1) << static_cast<int>(COMMAND_TYPE::LOCK_TARGET) |
                                size_t(1) << static_cast<int>(COMMAND_TYPE::RADAR_SWITCH) | 
                                size_t(1) << static_cast<int>(COMMAND_TYPE::SET_ROAD);

} // namespace command

// vehicle ID
using VID = uint64_t;
// side ID
using SID = uint16_t;

struct Block {
    constexpr static const char *token_list[] = {"length", "width", "height"};
    constexpr operator Vector3() const { return Vector3{length, width, height}; };
    constexpr double operator[](size_t i) const { return (&length)[i]; };
    double &operator[](size_t i) { return (&length)[i]; };
    double length, width, height;
    static Block make(){return Block{};}
};

struct Sphere {
    double r;
};

/**
 * @brief protection model, used in damage calculation of ammunition
 *
 */
struct ProtectionModel {
    constexpr static const char *token_list[] = {
        "armor_front", "armor_back",           "armor_side",    "armor_bottom",
        "armor_top",   "activeProtectionAmmo", "reactiveArmor", "coverageRate",
    };
    // armor thickness of each side
    double armor_front;
    double armor_back;
    double armor_side;
    double armor_bottom;
    double armor_top;
    // counter of active protection ammo
    int activeProtectionAmmo;
    // counter of reactive armor
    int reactiveArmor;
    // rate of surface area which covered by reactive armor
    double coverageRate;
    static ProtectionModel make(){return ProtectionModel{};}
};

// carhull
struct Hull {
    Vector3 velocity;
    Vector3 palstance;
};

// cardamage，越小越正常
enum class DAMAGE_LEVEL {
    N = 0,  // 正常
    M = 1,  // 中度毁伤
    K = 2,  // 失去功能
    KK = 3, // 失去结构
};

struct DamageModel {
    constexpr static const char *token_list[] = {"-damageLevel", "maxInfluence"};
    DAMAGE_LEVEL damageLevel;
    DAMAGE_LEVEL maxInfluence;
    static DamageModel make() { return DamageModel{DAMAGE_LEVEL::N, DAMAGE_LEVEL::KK}; }
};

// TODO: index to prevent redundant processing?
struct FireEvent {
    // constexpr static const char *token_list[] = {"weaponName", "target", "position", "velocity", "range"};
    std::string weaponName;
    // 目标点的世界坐标
    Vector3 target;
    // 炮弹位置的世界坐标
    Vector3 position;
    // 当前速度矢量的世界坐标
    Vector3 velocity;
    // 发射者和命中点的直线距离
    double range;
};

struct Direction {
    constexpr static const char *token_list[] = {"yaw", "pitch"};
    double yaw, pitch;
    double &operator[](size_t index) { return (&yaw)[index]; };
    const double &operator[](size_t index) const { return (&yaw)[index]; };
    static Direction make(){return Direction{};}
};

struct AngleZone {
    constexpr static const char *token_list[] = {"yawLeft", "yawRight", "pitchUp", "pitchDown"};
    double yawLeft, yawRight, pitchUp, pitchDown;
    double &operator[](size_t index) { return (&yawLeft)[index]; };
    const double &operator[](size_t index) const { return (&yawLeft)[index]; };
    bool containsDirection(const Direction& d) const {
        return d.yaw>=-yawLeft-INF_SMALL && d.yaw<=yawRight+INF_SMALL && d.pitch<=pitchUp+INF_SMALL && d.pitch>=-pitchDown-INF_SMALL;
    }
    static AngleZone make(){return AngleZone{};}
};

// carfireunit
enum class FIRE_UNIT_STATE {
    FREE,
    LOCK_TARGET, // must have target ID
    LOCK_DIRECTION,
    SINGLE_SHOOT, // must have target ID
    MULTI_SHOOT,  // must have target ID
    SEEK_TARGET,
};

struct Weapon {
    constexpr static const char *token_list[] = {"ammoType",        "ammoRemain", "reloadingTime",
                                                 "-reloadingState", "range",      "speed"};
    std::string ammoType;
    int ammoRemain;
    double reloadingTime;
    double reloadingState; ///< current remain reloading time
    double range;
    double speed;
    static Weapon make(){
        Weapon tmp;
        tmp.reloadingState = 0;
        return tmp;
    }
};

struct FireUnit {
    constexpr static const char *token_list[] = {"-state",      "-data", "fireZone", "rotateZone", "-presentDirection",
                                                 "rotateSpeed", "weapon"};
    FIRE_UNIT_STATE state;
    double data;                ///< target ID or angle
    AngleZone fireZone;         ///< [yawLeft, yawRight, pitchUp, pitchDown]
    AngleZone rotateZone;       ///< [yawLeft, yawRight, pitchUp, pitchDown]
    Direction presentDirection; ///< [yaw, pitch]
    Direction rotateSpeed;      ///< [yaw, pitch]
    Weapon weapon;
    static FireUnit make(){
        FireUnit tmp;
        tmp.state = FIRE_UNIT_STATE::FREE;
        tmp.data = 0;
        tmp.presentDirection = Direction{0, 0};
        return tmp;
    }
};

// carsensor
struct SensorData {
    constexpr static const char *token_list[] = {"type"};
    std::string type;
    static SensorData make(){return SensorData{};}
};

// carcommunication
struct CommunicationData {
    constexpr static const char* token_list[] = {"type"};
    std::string type;
    static CommunicationData make() { return CommunicationData{}; }
};

struct BaseInfo {
    constexpr static const char *token_list[] = {"type", "id", "side", "damageLevel"};
    enum class ENTITY_TYPE {
        CAR,
        UNKNOWN = -1,
    } type;
    VID id;
    SID side;
    DAMAGE_LEVEL damageLevel;
    static BaseInfo make(){return BaseInfo{};}
};

struct EntityInfo {
    // constexpr static const char *token_list[] = {"position", "velocity", "baseInfo"};
    Vector3 position;
    Vector3 velocity;
    BaseInfo baseInfo;
    // double lastScanned;
};

// component ID
using CID = size_t;

struct ScannedMemory : public std::map<VID, std::tuple<double, EntityInfo>> {};

struct WheelMotionParamList {
    constexpr static const char *token_list[] = {
        "-angle",
        "LENGTH",
        "MAX_ANGLE",
        "ROTATE_SPEED",
        "MAX_LINEAR_SPEED",
        "MAX_FRONT_ACCELERATION",
        "MAX_BRAKE_ACCELERATION",
        "MAX_LATERAL_ACCELERATION",
    };
    // 车轮转角，右为正
    double angle;
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
    static WheelMotionParamList make(){
        WheelMotionParamList tmp;
        tmp.angle = 0;
        return tmp;
    }
};

struct HitEventQueue : public std::vector<FireEvent> {};

struct FireEventQueue : public std::vector<FireEvent> {};

struct CommandBuffer : public std::map<command::COMMAND_TYPE, std::any> {};

struct EventBuffer : public std::unordered_map<std::string, std::any> {};

struct PathPlanningModel {
    std::vector<Vector3> route;
    Vector3 prePoint;
    size_t nextPoint;
};

using Components =
    ComponentManager<SingletonComponent<Coordinate, DamageModel, CommandBuffer, EventBuffer, HitEventQueue, FireEventQueue,
                       WheelMotionParamList, ScannedMemory, Sphere, Hull, SID, VID, PathPlanningModel>,
    NormalComponent<Coordinate, DamageModel, Block, ProtectionModel, FireUnit, SensorData, CommunicationData>>;

}; // namespace carphymodel