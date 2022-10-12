#pragma once

#include <string>
#include "../tools/datastructure.hpp"

class System{
public:
    virtual std::string getTag()=0;
    virtual std::string getConstrain()=0;
    virtual void Tick(double dt, Components& c)=0;
};