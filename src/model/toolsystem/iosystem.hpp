#pragma once

#include "../tools/datastructure.hpp"
#include "../framework/system.hpp"

namespace carPhyModel{

class InputSystem : public System{
public:
    InputSystem() = default;
    virtual void tick(double dt, Components& c) override {
        auto& input = *(c.getSpecificSingletonComponent<InputBuffer>()->p);
    };
    virtual ~InputSystem() = default;
};

class OutputSystem : public System{
public:
    OutputSystem() = default;
    virtual void tick(double dt, Components& c) override {
        auto& output = c.getSpecificSingletonComponent<OutputBuffer>().value();
    };
    virtual ~OutputSystem() = default;
};

}