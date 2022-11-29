#pragma once

#include "../framework/system.hpp"
// #include "src/model/sensors/radar.h"

namespace carPhyModel{

class SensorSystem : public System{
public:
    SensorSystem(){};
    virtual void tick(double dt, Components& c) override;
};

}