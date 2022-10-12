#ifndef __SRC_MODEL_AMMUNITIONS_AMMUNITIONFACTORY_H__
#define __SRC_MODEL_AMMUNITIONS_AMMUNITIONFACTORY_H__

#include "ammunition.hpp"
#include "AP.h"
#include "HE.h"
#include "HEAT.h"
#include "../framework/pattern.hpp"

using AmmunitionDamageFactory = Factory<AmmunitionDamage>;

inline AmmunitionDamageFactory::LUT AmmunitionDamageFactory::look_up_table{
    {"125mmGun", std::shared_ptr<AmmunitionDamage>(new HEDamage({5., 10., 30.}))},
};

#endif