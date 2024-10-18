#pragma once

#include "../tools/datastructure.hpp"

namespace carphymodel{

class Sensor{
public:
    virtual bool isDetectable(const Coordinate& self, const EntityInfo& e, const SensorData& sensor, const Hull& hull,
                            double jammerPower = 0.) const = 0;
    virtual ~Sensor(){};
};

};