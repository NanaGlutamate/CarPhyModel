#include <algorithm>

#include "../ammunition/ammunitionfactory.hpp"
#include "../tools/constant.hpp"
#include "cardamage.h"

namespace carphymodel {

void DamageSystem::tick(double dt, Components &c) {
    auto& hitEventQueue = c.getSpecificSingleton<HitEventQueue>().value();
    if (hitEventQueue.size() == 0) {
        return;
    }

    auto& totalDamage = c.getSpecificSingleton<DamageModel>().value();
    for (auto&& fireEvent : hitEventQueue) {
        auto ammunitionModel = AmmunitionDamageFactory::getProduct(fireEvent.weaponName);
        ammunitionModel->updateDamage(fireEvent, c);
    }

    DAMAGE_LEVEL tmp = totalDamage.damageLevel;
    for (auto&& [id, _damageModel] : c.getNormal<DamageModel>()) {
        auto currentDamage = _damageModel.damageLevel;
        if (currentDamage > _damageModel.maxInfluence) {
            currentDamage = _damageModel.maxInfluence;
        }
        if (tmp < currentDamage) {
            tmp = currentDamage;
        }
    }
    totalDamage.damageLevel = tmp;

    c.getSpecificSingleton<HitEventQueue>()->clear();
    return;
}

} // namespace carphymodel