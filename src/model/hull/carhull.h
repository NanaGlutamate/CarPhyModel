#pragma once

#include "../tools/datastructure.hpp"
#include "../environment.h"
#include "../framework/system.hpp"

// 标准坐标系：北东地与前右下

namespace carPhyModel{

class HullSystem : public System{
public:
    HullSystem() = default;
    virtual void tick(double dt, Components& c) override;
    virtual ~HullSystem() = default;
};

}