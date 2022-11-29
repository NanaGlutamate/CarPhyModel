#pragma once

#include "../tools/datastructure.hpp"

namespace carPhyModel{

class Sensor{
public:
    virtual bool isDetectable(const Coordinate& self, const EntityInfo& e, const SensorData& sensor, const Hull& hull) const=0;
    virtual ~Sensor(){};
};

};