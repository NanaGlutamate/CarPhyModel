#include "cardamage.h"
#include "../tools/constant.hpp"
#include "../ammunition/ammunitionfactory.hpp"

namespace carphymodel{

void DamageSystem::tick(double dt, Components& c){
    auto&& baseCoordinate = c.getSpecificSingletonComponent<Coordinate>().value();

    for(auto&& fireEvent : c.getSpecificSingletonComponent<HitEventQueue>().value()){
        auto ammunitionModel = AmmunitionDamageFactory::getProduct(fireEvent.weaponName).lock();
        // TODO: denifition of FireEvent modified
        auto localP = baseCoordinate.positionWorldToBody(fireEvent.position);
        auto localD = baseCoordinate.directionWorldToBody(fireEvent.velocity);
        // auto local_v = local_d;

        for(auto&& [id, _damageModel, _size, _coordinate] : c.getNormalComponents<DamageModel, Block, Coordinate>()){
            auto damageBefore = _damageModel.damageLevel;
            ammunitionModel->updateDamage(_damageModel, _size, _coordinate, localP, localD, localD, fireEvent.range);
        }
    }
    c.getSpecificSingletonComponent<HitEventQueue>().value().clear();
    return;
}

}