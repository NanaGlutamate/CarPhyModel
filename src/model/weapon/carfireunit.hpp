#pragma once

#include "../tools/datastructure.hpp"
#include "../framework/system.hpp"

namespace carphymodel{

class FireUnitSystem : public System{
public:
    FireUnitSystem() = default;
    virtual void tick(double dt, Components& c) override{};
    virtual ~FireUnitSystem() = default;
};

}