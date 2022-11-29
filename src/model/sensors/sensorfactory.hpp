#pragma once

#include "sensor.hpp"
#include "radar/myradar.h"
#include "../framework/pattern.hpp"

namespace carPhyModel{

class SensorFactory : public Factory<Sensor>{};

inline SensorFactory::LUT SensorFactory::look_up_table{
    {"radar", std::make_shared<MyRadar>()},
};

};