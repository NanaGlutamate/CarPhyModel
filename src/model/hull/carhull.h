#pragma once

#include "../tools/datastructure.hpp"
#include "../environment.h"
#include "../framework/system.hpp"

namespace carPhyModel{

class HullSystem : public System{
public:
    HullSystem() = default;
    virtual void tick(double dt, Components& c) override;
    virtual ~HullSystem() = default;
};

};