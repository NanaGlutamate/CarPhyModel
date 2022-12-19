#pragma once

#include "sensor.hpp"
#include "radar/myradar.h"
#include "../framework/pattern.hpp"

namespace carPhyModel{

struct SensorFactory : public Factory<SensorFactory, Sensor>{
    inline static LUT look_up_table{
        {"radar", std::make_shared<MyRadar>()}
    };
};

};