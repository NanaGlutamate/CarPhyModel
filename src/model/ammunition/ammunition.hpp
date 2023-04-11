#pragma once

#include <random>
#include "../tools/datastructure.hpp"

namespace carphymodel{

class AmmunitionDamage{
public:
    // virtual void updateDamage(DamageModel& pdm, const Block& size, const Coordinate& coordinate, const Vector3& pos, const Vector3& dir, const Vector3& vel, double range) const=0;
    virtual void updateDamage(const FireEvent& fireEvent, Components& c) const=0;
    virtual ~AmmunitionDamage(){};
};

}