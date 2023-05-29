#include <algorithm>

#include "../ammunition/ammunitionfactory.hpp"
#include "../tools/constant.hpp"
#include "cardamage.h"

namespace carphymodel {

void DamageSystem::tick(double dt, Components &c) {
    auto& hitEventQueue = c.getSpecificSingleton<HitEventQueue>().value();

    auto& totalDamage = c.getSpecificSingleton<DamageModel>();
    if (!totalDamage.has_value()) {
        int totalFunction = 0, totalStructure = 0;
        for (auto&& [id, _damageModel] : c.getNormal<DamageModel>()) {
            auto damageBefore = _damageModel.damageLevel;
            totalFunction += _damageModel.functionalWeight;
            totalStructure += _damageModel.structualWeight;
        }
        totalDamage = DamageModel{DAMAGE_LEVEL::N, totalFunction, totalStructure};
    }

    if (hitEventQueue.empty()) {
        return;
    }
    auto& baseCoordinate = c.getSpecificSingleton<Coordinate>().value();
    for (auto&& fireEvent : hitEventQueue) {
        auto ammunitionModel = AmmunitionDamageFactory::getProduct(fireEvent.weaponName);
        ammunitionModel->updateDamage(fireEvent, c);
    }

    int loseFunction = 0, loseStructure = 0;
    int mcnt = 0, kcnt = 0, kkcnt = 0, totalcnt = 0;
    if (totalDamage->functionalWeight || totalDamage->structualWeight) {
        for (auto&& [id, _damageModel] : c.getNormal<DamageModel>()) {
            auto damageBefore = _damageModel.damageLevel;
            totalcnt++;
            if (_damageModel.damageLevel == DAMAGE_LEVEL::M) {
                mcnt++;
                loseFunction += _damageModel.functionalWeight / 2;
            } else if (_damageModel.damageLevel == DAMAGE_LEVEL::K) {
                kcnt++;
                loseFunction += _damageModel.functionalWeight;
            } else if (_damageModel.damageLevel == DAMAGE_LEVEL::KK) {
                kkcnt++;
                loseFunction += _damageModel.functionalWeight;
                loseStructure += _damageModel.structualWeight;
            }
        }
    }

    DAMAGE_LEVEL tmp = DAMAGE_LEVEL::N;
    if (totalDamage->functionalWeight) {
        if (double(loseFunction) / totalDamage->functionalWeight >= 0.4) {
            tmp = DAMAGE_LEVEL::M;
        }
        if (double(loseFunction) / totalDamage->functionalWeight >= 0.7) {
            tmp = DAMAGE_LEVEL::K;
        }
    } else {
        if (totalcnt < mcnt * 2 + kcnt * 2){
            tmp = DAMAGE_LEVEL::M;
        }
        if (totalcnt < kcnt * 2){
            tmp = DAMAGE_LEVEL::K;
        }
    }
    
    if (totalDamage->structualWeight) {
        if (double(loseStructure) / totalDamage->structualWeight >= 0.8) {
            tmp = DAMAGE_LEVEL::KK;
        }
    } else {
        if (totalcnt < kkcnt * 2){
            tmp = DAMAGE_LEVEL::KK;
        }
    }
    totalDamage->damageLevel = tmp;

    c.getSpecificSingleton<HitEventQueue>()->clear();
    return;
}

} // namespace carphymodel