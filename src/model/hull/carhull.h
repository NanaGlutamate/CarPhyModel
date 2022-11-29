#pragma once

#include "../tools/datastructure.hpp"
#include "../environment.h"
#include "../framework/system.hpp"

namespace carPhyModel{

class CarHullSystem : public System{
private:
    EnvironmentInfoAgent env;
public:
    virtual void tick(double dt, Components& c) override;
};

};