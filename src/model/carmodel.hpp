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
    void init(const CSValueMap& initValue){
        using namespace std;
        if(auto it = initValue.find("filePath"); it!= initValue.end()){
            CarBuilder::buildFromFile(any_cast<string>(it->second), *this);
        }else{
            CarBuilder::buildFromFile("exampleCarV2.xml", *this);
        }
    }
    void setInput(const CSValueMap& inputValue){
        components.getSpecificSingletonComponent<InputBuffer>()->p = &inputValue;
        inputSystem.tick(0, components);
    }
    void tick(double dt){
        components.getSpecificSingletonComponent<OutputBuffer>()->clear();
        for(auto&& sys : systems){
            sys->tick(dt, components);
        }
        outputSystem.tick(0, components);
    }
    CSValueMap* getOutput(){
        return &(components.getSpecificSingletonComponent<OutputBuffer>().value());
    }
private:
    CarModel(const CarModel&) = default;

    Components components;

    inline static InputSystem inputSystem{};
    inline static OutputSystem outputSystem{};
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