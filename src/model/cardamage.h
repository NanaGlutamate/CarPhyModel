#ifndef __SRC_MODEL_CARDAMAGE_H__
#define __SRC_MODEL_CARDAMAGE_H__

#include "tools/datastructure.hpp"
#include "framework/entitysynthesizer.hpp"
#include "ammunitions/ammunitionfactory.hpp"

class DamageSystem{
private:
    AmmunitionDamageFactory* adf;
public:
    DamageSystem():adf(AmmunitionDamageFactory::getInstance()){};
    void tick(double dt, DamageList& damage, const Coordinate& baseCoordinate, const CoordinateList& coordinate, const FireEvent& fireEvent);
};

#endif