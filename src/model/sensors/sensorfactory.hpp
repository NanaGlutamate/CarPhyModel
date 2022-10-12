#pragma once

#include "sensor.hpp"
#include "radar/myradar.h"
#include "../framework/pattern.hpp"

using SensorFactory = Factory<Sensor>;

inline SensorFactory::LUT SensorFactory::look_up_table{
    {"radar", std::shared_ptr<Sensor>(new MyRadar())},
};