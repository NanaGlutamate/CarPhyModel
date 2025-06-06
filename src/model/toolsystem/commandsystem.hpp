#pragma once

#include "../tools/datastructure.hpp"
#include "../framework/system.hpp"

namespace carphymodel{

class CommandSystem : public System{
public:
    CommandSystem() = default;
    virtual void tick(double dt, Components& c) override {};
    virtual ~CommandSystem() = default;
};

}