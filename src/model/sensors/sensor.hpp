#ifndef __SRC_MODEL_SENSORS_SENSOR_H__
#define __SRC_MODEL_SENSORS_SENSOR_H__

#include "../tools/datastructure.hpp"

class Sensor{
public:
    virtual bool isDetectable(const Coordinate& self, const EntityInfo& e, const SensorData& sensor, const Hull& hull) const=0;
};

#endif