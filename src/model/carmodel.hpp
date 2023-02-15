#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <any>
#include "tools/datastructure.hpp"
#include "framework/system.hpp"
#include "toolsystem/iosystem.hpp"

namespace carphymodel{

class CarModel{
public:
    friend class CarBuilder;
    CarModel() = default;
    using CSValueMap = std::unordered_map<std::string, std::any>;
    void tick(double dt){
        components.getSpecificSingleton<OutputBuffer>()->clear();
        for(auto&& sys : systems){
            sys->tick(dt, components);
        }
    }
    CSValueMap* getOutput(){
        return &(components.getSpecificSingleton<OutputBuffer>().value());
    }
    Components components;
private:
    CarModel(const CarModel&) = default;
    inline static std::vector<std::unique_ptr<System>> systems{};
};

}

// class CarModel{
// public:
//     friend class CarBuilder;

//     CarModel()=default;
//     void tick(double dt);

//     //IO buffer
//     Vector3 expDirection; double expSpeed;
// //private:
//     //ID
//     VID vid;
//     //传感器扫描到的实体信息缓存
//     ScannedMemory scannedMemory;

//     //各部分系统类
//     CarHullSystem hullsys;
//     ProtectionModel protectionsys;
//     DamageSystem damagesys;
//     FireControlSystem firesys;
//     SensorSystem sensorsys;

//     //各部分组件数据类
//     Hull hull;
//     WheelMotionParamList params;
//     Coordinate baseCoordinate;
//     CoordinateList coordinate;
//     ProtectionList protection;
//     DamageList damage;
//     FireUnitList fireUnit;
//     SensorList sensor;
// };