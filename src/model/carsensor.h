#ifndef __CARSENSOR_H__
#define __CARSENSOR_H__

#include <algorithm>
#include <tuple>
#include "tools/datastructure.hpp"
#include "sensors/sensorfactory.hpp"
#include "framework/system.hpp"
// #include "src/model/sensors/radar.h"

class SensorSystem{
public:
    SensorSystem(){};
    void tick(double dt, ScannedMemory& scannedMemory, const SensorList& sensorData, const DamageList& damage, const Coordinate& baseCoordinate, const Hull& hull);
};

#endif