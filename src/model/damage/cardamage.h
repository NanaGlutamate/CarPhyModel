#pragma once

#include "../tools/datastructure.hpp"
#include "../framework/system.hpp"

namespace carphymodel{

class DamageSystem : public System{
private:
public:
    DamageSystem() = default;
    virtual void tick(double dt, Components& c) override;
    virtual ~DamageSystem() = default;
};

};