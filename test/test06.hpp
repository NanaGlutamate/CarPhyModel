#include "basetest.h"

namespace test06{

int test_main(){
    AmmunitionDamageFactory* t = AmmunitionDamageFactory::getInstance();
    auto p = t->getAmmunitionDamage("125mmGun");
    auto p2 = dynamic_cast<const HEDamage*>(p);
    auto p3 = dynamic_cast<const APDamage*>(p);
    return 0;
};

}