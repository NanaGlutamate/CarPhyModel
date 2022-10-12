#ifndef __SRC_MODEL_CARMODEL_H__
#define __SRC_MODEL_CARMODEL_H__

#include "tools/datastructure.hpp"
#include "tools/coordinate.hpp"
#include "carhull.h"
#include "carprotection.h"
#include "cardamage.h"
#include "carfireunit.h"
#include "sensors/sensor.hpp"

class CarModel{
public:
    friend class CarBuilder;

    CarModel()=default;
    void tick(double dt);

    //IO buffer
    Vector3 expDirection; double expSpeed;
//private:
    //ID
    VID vid;
    //传感器扫描到的实体信息缓存
    ScannedMemory scannedMemory;

    //各部分系统类
    CarHullSystem hullsys;
    ProtectionModel protectionsys;
    DamageSystem damagesys;
    FireControlSystem firesys;
    SensorSystem sensorsys;

    //各部分组件数据类
    Hull hull;
    WheelMotionParamList params;
    Coordinate baseCoordinate;
    CoordinateList coordinate;
    ProtectionList protection;
    DamageList damage;
    FireUnitList fireUnit;
    SensorList sensor;
};

#endif