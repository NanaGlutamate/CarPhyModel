#pragma once

#include "../framework/system.hpp"
// #include "src/model/sensors/radar.h"

namespace carphymodel{

class SensorSystem : public System{
public:
    SensorSystem() = default;
    virtual void tick(double dt, Components& c) override;
    virtual ~SensorSystem() override = default;
};

}