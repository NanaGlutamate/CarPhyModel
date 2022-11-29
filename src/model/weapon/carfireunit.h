#ifndef __SRC_MODEL_CARFIREUNIT_H__
#define __SRC_MODEL_CARFIREUNIT_H__

#include "../tools/datastructure.hpp"
#include "../framework/system.hpp"

namespace carPhyModel{

class FireControlSystem : public System{
public:
    FireControlSystem() = default;
    virtual void tick(double dt, Components& c) override;
};

}

#endif