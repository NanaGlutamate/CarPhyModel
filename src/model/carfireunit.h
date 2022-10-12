#ifndef __SRC_MODEL_CARFIREUNIT_H__
#define __SRC_MODEL_CARFIREUNIT_H__

#include "tools/datastructure.hpp"

class FireControlSystem{
public:
    FireControlSystem(){};
    void setOrder();
    void tick(double dt, FireUnitList& fireUnit, const ScannedMemory& entityInfos, const DamageList& damage);
};

#endif