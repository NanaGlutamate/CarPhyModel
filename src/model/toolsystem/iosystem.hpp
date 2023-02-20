#pragma once

#include "../tools/datastructure.hpp"
#include "../framework/system.hpp"

namespace carphymodel{

class PrepareSystem : public System{
public:
    PrepareSystem() = default;
    virtual void tick(double dt, Components& c) override {
        c.getSpecificSingleton<EventBuffer>().value().clear();
    };
    virtual ~PrepareSystem() = default;
};

class OutputSystem : public System{
public:
    OutputSystem() = default;
    virtual void tick(double dt, Components& c) override {
        c.getSpecificSingleton<CommandBuffer>().value().clear();
    };
    virtual ~OutputSystem() = default;
};

}
