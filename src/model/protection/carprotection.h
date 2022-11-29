#pragma once

#include "../tools/datastructure.hpp"
#include "../framework/system.hpp"

namespace carPhyModel{

class ProtectionSystem : public System{
public:
    ProtectionSystem(){};
    virtual void tick(double dt, Components& c);
};

}