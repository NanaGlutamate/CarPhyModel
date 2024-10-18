#include <algorithm>
#include <cmath>
#include <map>
#include <ranges>
#include <tuple>
#include <vector>
#include <fstream>
#include "../tools/constant.hpp"
#include "../tools/myrandom.hpp"
#include "AP.h"
#include "collision.hpp"

namespace carphymodel {

void APDamage::updateDamage(const FireEvent& fireEvent, Components& c) const {

    auto velocity = c.getSpecificSingleton<Coordinate>()->directionWorldToBody(fireEvent.velocity);
    auto position = c.getSpecificSingleton<Coordinate>()->positionWorldToBody(fireEvent.position);
    std::ofstream logFile("interception_log.txt", std::ios::app);
    // calculate all collision with protection model
    std::vector<std::tuple<ProtectionModel&, IntersectionInfo>> collisionWithProtection;
    for (auto&& [id, protection, block, coordinate] : c.getNormal<ProtectionModel, Block, Coordinate>()) {
        auto inter = rayCollision(velocity, position, block, coordinate);
        if (!inter.isCollision()) {
            continue;
        }
        collisionWithProtection.emplace_back(protection, inter);
    }
    if (collisionWithProtection.empty()) {
        return;
    }
    std::ranges::sort(collisionWithProtection, {}, [](auto& a) { return std::get<1>(a).minDepth; });

    // max depth, after which the projectile will disarmed
    double maxDepth = INF_BIG;
    // // depth of first protection
    // double minDepth;
    auto it = piercingAbilityTable.lower_bound(fireEvent.range);
    if (it == piercingAbilityTable.end()) {
        return;
    }
    double piercing = it->second;

    // process collision with protection model
    for (auto& [protection, inter] : collisionWithProtection) {
        // TODO: log?
        if (protection.activeProtectionAmmo) {
            protection.activeProtectionAmmo--;
            if ((fireEvent.weaponName == "AP" || fireEvent.weaponName == "HE") &&
                rand() <= protection.Interception_probability1) {
                maxDepth = inter.minDepth;
                //for test
                logFile << "HE interception result: success" << std::endl; // 写入文件
                //std::cout << "interception result: success" << std::endl;
                break;
            }else if ((fireEvent.weaponName == "antitankmissile" || fireEvent.weaponName == "rocket") &&
                rand() <= protection.Interception_probability2) {
                //protection.activeProtectionAmmo--;
                maxDepth = inter.minDepth;
                logFile << "HE interception result: success" << std::endl; // 写入文件
                //std::cout << "interception result: success" << std::endl;
                break;
            }
            logFile << "HE interception result: fail" << std::endl; // 写入文件
        /*maxDepth = inter.minDepth;*/
        /*break;*/
    }
    if (protection.reactiveArmor && randEvent(protection.coverageRate)) {
        protection.reactiveArmor--;
        maxDepth = inter.minDepth;
        protection.coverageRate *= protection.reactiveArmor / (protection.reactiveArmor + 1);
        break;
    }
    double armor = getSideArmor(protection, inter.hitSurface);
    // negative unit normal vector of each surface
    auto direction = -1 * directionVector[static_cast<unsigned int>(inter.hitSurface)];
    // armor / cos(theta)
    piercing -= armor / ((velocity.dot(direction)) / velocity.norm());
    //for test
    /*std::cout << "AP damage: at " << fireEvent.range << "m"
         << "piercing damage: " << piercing << std::endl;*/

    if (piercing <= 0.) {
        maxDepth = inter.minDepth;
        break;
    }
    }

    for (auto &&[id, damage, block, coordinate] : c.getNormal<DamageModel, Block, Coordinate>()) {
        if(damage.damageLevel == DAMAGE_LEVEL::KK){
            continue;
        }
        auto inter = rayCollision(velocity, position, block, coordinate);
        if (!inter.isCollision() || inter.minDepth >= maxDepth) {
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