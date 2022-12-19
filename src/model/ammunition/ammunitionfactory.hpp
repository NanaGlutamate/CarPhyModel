#pragma once

#include <memory>
#include "ammunition.hpp"
#include "AP.h"
#include "HE.h"
#include "HEAT.h"
#include "../framework/pattern.hpp"

namespace carPhyModel{

struct AmmunitionDamageFactory : public Factory<AmmunitionDamageFactory, AmmunitionDamage>{
    inline static LUT look_up_table{
        {"125mmGun", std::make_shared<HEDamage>(std::array<double, 3>{5., 10., 30.})},
    };
};

};