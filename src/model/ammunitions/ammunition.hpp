#ifndef __SRC_MODEL_AMMUNITIONS_AMMUNITION_H__
#define __SRC_MODEL_AMMUNITIONS_AMMUNITION_H__

#include <random>
#include "../tools/datastructure.hpp"

class AmmunitionDamage{
public:
    virtual void updateDamage(PartDamageModel& pdm, const Coordinate& coordinate, const Vector3& pos, const Vector3& dir, const Vector3& vel, double range) const=0;
};

#endif