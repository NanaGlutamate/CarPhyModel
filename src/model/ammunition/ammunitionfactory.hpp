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
        {"125mmAP", std::make_shared<APDamage>(std::map<double, double>{{1e10, 0.8}})},
        {"HE", std::make_shared<HEDamage>(std::array<double, 3>{5., 10., 15.})},
        {"AP", std::make_shared<APDamage>(std::map<double, double>{{1000, 0.9}, {2000, 0.6},{3000, 0.3}, {1e10, 0.01}})},
        {"antitankmissile", std::make_shared<OTHERDamage>(std::map<double, double>{{100, 0.9}, {1e10, 0.8}})},
        {"rocket", std::make_shared<OTHERDamage>(std::map<double, double>{{100, 0.9}, {1e10, 0.8}})},
        {"verticalmissile", std::make_shared<OTHERDamage>(std::map<double, double>{{100, 0.9}, {1e10, 0.8}})},
        {"machinegun762", std::make_shared<OTHERDamage>(std::map<double, double>{{100, 0.9}, {1e10, 0.8}})},
        {"machinegun1270", std::make_shared<OTHERDamage>(std::map<double, double>{{100, 0.9}, {1e10, 0.8}})},
    };
};

};