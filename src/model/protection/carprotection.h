#pragma once

#include "../tools/datastructure.hpp"
#include "../framework/system.hpp"

namespace carphymodel{

class ProtectionSystem : public System{
public:
    ProtectionSystem() = default;
    virtual void tick(double dt, Components& c){};
    virtual ~ProtectionSystem() = default;
};

}