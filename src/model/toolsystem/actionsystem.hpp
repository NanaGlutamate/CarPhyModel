#pragma once

#include "../tools/datastructure.hpp"
#include "../framework/system.hpp"

namespace carphymodel{

class ActionSystem : public System{
public:
    ActionSystem() = default;
    virtual void tick(double dt, Components& c) override {};
    virtual ~ActionSystem() = default;
};

}