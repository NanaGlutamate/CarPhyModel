#pragma once

#include "../sensor.hpp"
#include "../../tools/datastructure.hpp"
#include <unordered_map>
#include <memory>
//include <iostream>

namespace carphymodel{

class MyRadar : public Sensor {
public:
    virtual bool isDetectable(const Coordinate& self, const EntityInfo& e, const SensorData& sensor, const Hull& hull) const override;
};

};