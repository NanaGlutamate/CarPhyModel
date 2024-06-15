#pragma once

#include <memory>
#include "ammunition.hpp"
#include "AP.h"
#include "HE.h"
#include "HEAT.hpp"
#include "../framework/pattern.hpp"

namespace carphymodel{

struct AmmunitionDamageFactory : public Factory<AmmunitionDamageFactory, AmmunitionDamage>{
    inline static LUT look_up_table{
        {"125mmHE", std::make_shared<HEDamage>(std::array<double, 3>{5., 10., 30.})},
        {"125mmAP", std::make_shared<APDamage>(std::map<double, double>{{1e10, 800}})},
    };
};

};