#pragma once

#include "../sensor.hpp"
#include "../../tools/datastructure.hpp"
#include <unordered_map>
#include <memory>
//include <iostream>

namespace carphymodel{

class MyRadar final : public Sensor {
public:
    virtual bool isDetectable(const Coordinate& self, const EntityInfo& e, const SensorData& sensor, const Hull& hull,
                              double jammerPower = 0.) const override;
};

};