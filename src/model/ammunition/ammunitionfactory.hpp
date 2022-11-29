#pragma once

#include <memory>
#include "ammunition.hpp"
#include "AP.h"
#include "HE.h"
#include "HEAT.h"
#include "../framework/pattern.hpp"

namespace carPhyModel{

class AmmunitionDamageFactory : public Factory<AmmunitionDamage>{};

inline AmmunitionDamageFactory::LUT AmmunitionDamageFactory::look_up_table{
    {"125mmGun", std::make_shared<HEDamage>(std::array<double, 3>{5., 10., 30.})},
};

};