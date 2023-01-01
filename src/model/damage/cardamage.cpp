#include "cardamage.h"
#include "../tools/constant.hpp"
#include "../ammunition/ammunitionfactory.hpp"

namespace carPhyModel{

void DamageSystem::tick(double dt, Components& c){
    auto&& baseCoordinate = c.getSpecificSingletonComponent<Coordinate>().value();

    for(auto&& fireEvent : c.getSpecificSingletonComponent<HitEventQueue>().value()){
        auto ammunitionModel = AmmunitionDamageFactory::getProduct(fireEvent.weaponName).lock();
        auto local_p = baseCoordinate.positionWorldToBody(fireEvent.position);
        auto local_d = baseCoordinate.directionWorldToBody(fireEvent.direction);
        auto local_v = baseCoordinate.directionWorldToBody(fireEvent.velocity);

        for(auto&& [id, _damage_model, _size, _coordinate] : c.getNormalComponents<PartDamageModel, Block, Coordinate>()){
            ammunitionModel->updateDamage(_damage_model, _size, _coordinate, local_p, local_d, local_v, fireEvent.range);
        }
    }
    return;
}

}