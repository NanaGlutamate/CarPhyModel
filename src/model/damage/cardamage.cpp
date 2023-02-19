#include <algorithm>

#include "cardamage.h"
#include "../tools/constant.hpp"
#include "../ammunition/ammunitionfactory.hpp"

namespace{

template<typename Ty>
size_t argmaxRight(Ty list){
    size_t index = 0;
    auto value = list[0];
    for(size_t i = 1; i < list.size(); ++i){
        if(list[i] > value){
            value = list[i];
            index = i;
        }
    }
    return index;
}

}

namespace carphymodel{

void DamageSystem::tick(double dt, Components& c){
    auto&& baseCoordinate = c.getSpecificSingleton<Coordinate>().value();

    std::array<int, static_cast<std::underlying_type_t<DAMAGE_LEVEL>>(DAMAGE_LEVEL::KK) + 1> damageLevelCounter = {}; 

    for(auto&& fireEvent : c.getSpecificSingleton<HitEventQueue>().value()){
        auto ammunitionModel = AmmunitionDamageFactory::getProduct(fireEvent.weaponName);
        auto localP = baseCoordinate.positionWorldToBody(fireEvent.position);
        auto localD = baseCoordinate.directionWorldToBody(fireEvent.velocity);

        for(auto&& [id, _damageModel, _size, _coordinate] : c.getNormal<DamageModel, Block, Coordinate>()){
            auto damageBefore = _damageModel.damageLevel;
            if(damageBefore != DAMAGE_LEVEL::KK){
                ammunitionModel->updateDamage(_damageModel, _size, _coordinate, localP, localD, localD, fireEvent.range);
            }
            damageLevelCounter[static_cast<std::underlying_type_t<DAMAGE_LEVEL>>(_damageModel.damageLevel)]++;
        }
    }

    c.getSpecificSingleton<DamageModel>()->damageLevel = static_cast<DAMAGE_LEVEL>(argmaxRight(damageLevelCounter));

    c.getSpecificSingleton<HitEventQueue>().value().clear();
    return;
}

}