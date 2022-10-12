#ifndef __SRC_MODEL_CARHULL_H__
#define __SRC_MODEL_CARHULL_H__

#include "tools/datastructure.hpp"
#include "environment.h"
#include "framework/system.hpp"

class CarHullSystem : System{
private:
    EnvironmentInfoAgent env;
public:
    virtual void Tick(double dt, Components& c) override;
};

#endif