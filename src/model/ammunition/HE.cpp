#include "HE.h"
#include "../tools/constant.hpp"
#include "collision.hpp"

namespace {

using namespace carphymodel;

inline void update(DAMAGE_LEVEL &tar, DAMAGE_LEVEL newDamage) {
    if (newDamage > tar) {
        tar = newDamage;
    }
}

} // namespace

namespace carphymodel {

// no damage if any armor between explosion point and damage model
void HEDamage::updateDamage(const FireEvent &fireEvent, Components &c) const {

    auto& carCoordinate = c.getSpecificSingleton<Coordinate>().value();
    auto velocity = carCoordinate.directionWorldToBody(fireEvent.velocity);
    auto position = carCoordinate.positionWorldToBody(fireEvent.position);

    for (auto &&[id, protection] : c.getNormal<ProtectionModel>()) {
        if (protection.activeProtectionAmmo) {
            protection.activeProtectionAmmo--;
            return;
        }
    }

    // depth when projectile explode, which is the depth when meet first block
    double depthExplode = INF_BIG;
    for (auto &&[id, block, coordinate] : c.getNormal<Block, Coordinate>()) {
        auto inter = rayCollision(velocity, position, block, coordinate);
        if (!inter.isCollision()) {
            continue;
        }
        depthExplode = fmin(depthExplode, inter.minDepth);
    }
    Vector3 explosionPoint;
    if (depthExplode == INF_BIG) {
        explosionPoint = carCoordinate.positionWorldToBody(fireEvent.target);
    } else {
        explosionPoint = position + velocity * depthExplode;
    }

    for (auto &&[id, damage, coordinate] : c.getNormal<DamageModel, Coordinate>()) {
        if (damage.damageLevel == DAMAGE_LEVEL::KK) {
            continue;
        }
        // use lambda to interupt damage calculation if protection available
        [&]() {
            for (auto &&[_id, _protection, _block, _coordinate] : c.getNormal<ProtectionModel, Block, Coordinate>()) {
                auto inter = segmentCollision(position, coordinate.position, _block, _coordinate);
                if (inter.isCollision()) {
                    return;
                }
            }
            auto radius = (explosionPoint - coordinate.position).norm();
            if (radius <= damageTable[0]) {
                damage.damageLevel = DAMAGE_LEVEL::KK;
            } else if (radius <= damageTable[1]) {
                update(damage.damageLevel, DAMAGE_LEVEL::K);
            } else if (radius <= damageTable[2]) {
                update(damage.damageLevel, DAMAGE_LEVEL::M);
            }
        }();
    }
}

} // namespace carphymodel