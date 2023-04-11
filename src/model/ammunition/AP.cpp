#include <algorithm>
#include <cmath>
#include <map>
#include <ranges>
#include <tuple>
#include <vector>

#include "../tools/constant.hpp"
#include "../tools/myrandom.hpp"
#include "AP.h"
#include "collision.hpp"

namespace carphymodel {

void APDamage::updateDamage(const FireEvent &fireEvent, Components &c) const {

    auto velocity = c.getSpecificSingleton<Coordinate>()->directionWorldToBody(fireEvent.velocity);
    auto position = c.getSpecificSingleton<Coordinate>()->positionWorldToBody(fireEvent.position);

    // calculate all collision with protection model
    std::vector<std::tuple<ProtectionModel &, IntersectionInfo>> collisionWithProtection;
    for (auto [id, protection, block, coordinate] : c.getNormal<ProtectionModel, Block, Coordinate>()) {
        auto inter = rayCollision(velocity, position, block, coordinate);
        if (!inter.isCollision()) {
            continue;
        }
        collisionWithProtection.emplace_back(protection, inter);
    }
    std::ranges::sort(collisionWithProtection, {}, [](auto &a) { return std::get<1>(a).depthMin; });

    // max depth, after which the projectile will disarmed
    double depthMax;
    // // depth of first protection
    // double depthMin;
    double piercing = piercingAbility;

    // process collision with protection model
    for (auto &[protection, inter] : collisionWithProtection) {
        // TODO: log?
        if (protection.activeProtectionAmmo) {
            protection.activeProtectionAmmo--;
            depthMax = inter.depthMin;
            break;
        }
        if (protection.reactiveArmor && rand() < protection.coverageRate) {
            protection.reactiveArmor--;
            depthMax = inter.depthMin;
            protection.coverageRate *= protection.reactiveArmor / (protection.reactiveArmor + 1);
            break;
        }
        double armor = getSideArmor(protection)[static_cast<unsigned int>(inter.hitSurface)];
        // nega unit normal vector of each surface
        auto direction = -1 * directionVector[static_cast<unsigned int>(inter.hitSurface)];
        // armor / cos(theta)
        piercing -= armor / ((velocity.dot(direction)) / velocity.norm());
        if (piercing <= 0.) {
            depthMax = inter.depthMin;
            break;
        }
    }

    for (auto [id, damage, block, coordinate] : c.getNormal<DamageModel, Block, Coordinate>()) {
        if(damage.damageLevel == DAMAGE_LEVEL::KK){
            continue;
        }
        auto inter = rayCollision(velocity, position, block, coordinate);
        if (!inter.isCollision() || inter.depthMin >= depthMax) {
            continue;
        }
        if (damage.damageLevel == DAMAGE_LEVEL::K){
            damage.damageLevel = DAMAGE_LEVEL::KK;
        }else{
            damage.damageLevel = DAMAGE_LEVEL::K;
        }
    }
}

} // namespace carphymodel