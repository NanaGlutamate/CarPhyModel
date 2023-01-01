#pragma once

#include "../tools/datastructure.hpp"
#include "../framework/system.hpp"

namespace carPhyModel{

class BallisticSystem : public System{
public:
    BallisticSystem() = default;
    virtual void tick(double dt, Components& c) override {};
    virtual ~BallisticSystem() = default;
};

}