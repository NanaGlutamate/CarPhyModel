#include <algorithm>

#include "../ammunition/ammunitionfactory.hpp"
#include "../tools/constant.hpp"
#include "cardamage.h"

namespace {

template <typename Ty> size_t argmaxRight(Ty list) {
    size_t index = 0;
    auto value = list[0];
    for (size_t i = 1; i < list.size(); ++i) {
        if (list[i] > value) {
            value = list[i];
            index = i;
        }
    }
    return index;
}

} // namespace

namespace carphymodel {

void DamageSystem::tick(double dt, Components &c) {
    auto &&baseCoordinate = c.getSpecificSingleton<Coordinate>().value();

    int loseFunction = 0, loseStructure = 0;
    int totalFunction = 0, totalStructure = 0;

    for (auto &&fireEvent : c.getSpecificSingleton<HitEventQueue>().value()) {
        auto ammunitionModel = AmmunitionDamageFactory::getProduct(fireEvent.weaponName);

        ammunitionModel->updateDamage(fireEvent, c);
    }

    for (auto &&[id, _damageModel] : c.getNormal<DamageModel>()) {
        auto damageBefore = _damageModel.damageLevel;
        totalFunction += _damageModel.functionalWeight;
        totalStructure += _damageModel.structualWeight;
        if (_damageModel.damageLevel == DAMAGE_LEVEL::M) {
            loseFunction += _damageModel.functionalWeight / 2;
        } else if (_damageModel.damageLevel == DAMAGE_LEVEL::K) {
            loseFunction += _damageModel.functionalWeight;
        } else if (_damageModel.damageLevel == DAMAGE_LEVEL::KK) {
            loseFunction += _damageModel.functionalWeight;
            loseStructure += _damageModel.structualWeight;
        }
    }

    DAMAGE_LEVEL tmp = DAMAGE_LEVEL::N;
    if (totalFunction != 0 && double(loseFunction) / totalFunction >= 0.4)
        tmp = DAMAGE_LEVEL::M;
    if (totalFunction != 0 && double(loseFunction) / totalFunction >= 0.7)
        tmp = DAMAGE_LEVEL::K;
    if (totalStructure != 0 && double(loseStructure) / totalStructure >= 0.8)
        tmp = DAMAGE_LEVEL::KK;

    auto &totalDamage = c.getSpecificSingleton<DamageModel>();
    if (!totalDamage.has_value()) {
        totalDamage = DamageModel{tmp, totalFunction, totalStructure};
    } else {
        totalDamage->damageLevel = tmp;
    }

    c.getSpecificSingleton<HitEventQueue>()->clear();
    return;
}

} // namespace carphymodel