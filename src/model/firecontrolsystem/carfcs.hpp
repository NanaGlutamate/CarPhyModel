#pragma once

#include <any>
#include <functional>
#include <set>
#include <tuple>

#include "../framework/system.hpp"
#include "../tools/constant.hpp"
#include "../tools/datastructure.hpp"
#include "../tools/myassert.hpp"

namespace carphymodel {

class FireControlSystem : public System {
  public:
    FireControlSystem() = default;
    virtual void tick(double dt, Components &c) override {
        using namespace command;
        using namespace std;

        applyCommand(dt, c);

        auto aimedAndInRange = updateFireUnitState(dt, c);

        setFire(dt, c, aimedAndInRange);
    };
    virtual ~FireControlSystem() = default;

  private:
    FireUnit &getNthFireUnit(size_t n, Components &c) {
        size_t cnt = 0;
        for (auto &&[id, fireUnit] : c.getNormal<FireUnit>()) {
            if (cnt == n) {
                return fireUnit;
            }
            ++cnt;
        }
        my_assert(false, "Invalid Weapon Index " + std::to_string(n));
        return get<1>(*c.getNormal<FireUnit>().begin());
    }
    void applyCommand(double dt, Components &c) {
        using namespace command;
        using namespace std;

        const static map<COMMAND_TYPE, FIRE_UNIT_STATE> commands{
            {COMMAND_TYPE::SHOOT, FIRE_UNIT_STATE::SINGLE_SHOOT},
            {COMMAND_TYPE::LOCK_DIRECTION, FIRE_UNIT_STATE::LOCK_DIRECTION},
            {COMMAND_TYPE::LOCK_TARGET, FIRE_UNIT_STATE::LOCK_TARGET},
            {COMMAND_TYPE::UNLOCK, FIRE_UNIT_STATE::FREE},
        };

        for (auto &[k, v] : c.getSpecificSingleton<CommandBuffer>().value()) {
            if (auto it = commands.find(k); it != commands.end()) {
                auto [index, param] = any_cast<tuple<double, double>>(v);
                auto &tmp = getNthFireUnit((size_t)index, c);
                tmp.state = it->second;
                tmp.data = param;
            } else if (k == COMMAND_TYPE::FREE_SHOOT) {
                for (auto &&[id, fireUnit] : c.getNormal<FireUnit>()) {
                    if (fireUnit.state == FIRE_UNIT_STATE::FREE || fireUnit.state == FIRE_UNIT_STATE::LOCK_DIRECTION ||
                        fireUnit.state == FIRE_UNIT_STATE::SEEK_TARGET) {
                        fireUnit.state = FIRE_UNIT_STATE::SEEK_TARGET;
                    } else {
                        fireUnit.state = FIRE_UNIT_STATE::MULTI_SHOOT;
                    }
                }
                break;
            } else if (k == COMMAND_TYPE::STOP_SHOOT) {
                for (auto &&[id, fireUnit] : c.getNormal<FireUnit>()) {
                    fireUnit.state = FIRE_UNIT_STATE::FREE;
                }
                break;
            }
        }
    }
    std::set<std::reference_wrapper<FireUnit>> updateFireUnitState(double dt, Components &c) {
        // scanned memory中存储的位置所在的帧与目标解算毁伤事件所在帧相差的帧数
        constexpr auto lagFrameCounter = 1;
        std::set<std::reference_wrapper<FireUnit>> ret;

        // 0. data preparing
        const auto &baseCoordinate = c.getSpecificSingleton<Coordinate>().value();
        const auto &selfPosition = baseCoordinate.position;
        auto &scannedMemory = c.getSpecificSingleton<ScannedMemory>();
        if (scannedMemory.value().size() == 0) {
            // no target, clear fire unit state
            for (auto &&[id, fireUnit] : c.getNormal<FireUnit>()) {
                fireUnit.state = FIRE_UNIT_STATE::FREE;
            }
            return;
        }
        size_t nearstTarget = 0;
        double minDistance = -1;
        for (auto &[id, tar] : scannedMemory.value()) {
            if (!isTargetAvailable(tar)) {
                continue;
            }
            auto dis = (std::get<1>(tar).position - selfPosition).norm();
            if (dis < minDistance || minDistance == -1) {
                minDistance = dis;
                nearstTarget = id;
            }
        }

        // main loop
        for (auto &&[id, damageModel, fireUnit, coordinate] : c.getNormal<DamageModel, FireUnit, Coordinate>()) {

            if (damageModel.damageLevel != DAMAGE_LEVEL::N || damageModel.damageLevel != DAMAGE_LEVEL::M) {
                // destroyed
                continue;
            }

            // 1. update reloading
            fireUnit.weapon.reloadingState -= dt;
            if (fireUnit.weapon.reloadingState < 0) {
                fireUnit.weapon.reloadingState = 0;
            }
            if (fireUnit.state == FIRE_UNIT_STATE::FREE) {
                continue;
            }

            // 2. make target avaliable
            if (fireUnit.state == FIRE_UNIT_STATE::MULTI_SHOOT || fireUnit.state == FIRE_UNIT_STATE::SINGLE_SHOOT ||
                fireUnit.state == FIRE_UNIT_STATE::LOCK_TARGET) {
                auto it = scannedMemory.value().find(fireUnit.data);
                if (it == scannedMemory.value().end() || !isTargetAvailable(it->second)) {
                    if (fireUnit.state == FIRE_UNIT_STATE::MULTI_SHOOT) {
                        fireUnit.data = static_cast<double>(nearstTarget);
                    } else {
                        fireUnit.state = FIRE_UNIT_STATE::FREE;
                        continue;
                    }
                }
            }
            if (fireUnit.state == FIRE_UNIT_STATE::SEEK_TARGET) {
                fireUnit.data = static_cast<double>(nearstTarget);
                fireUnit.state = FIRE_UNIT_STATE::MULTI_SHOOT;
            }

            // 3. rotate turret
            double expectDirection, expectPitch;
            bool inRange = false;
            if (fireUnit.state == FIRE_UNIT_STATE::LOCK_DIRECTION) {
                auto globalDirection = Vector3{cos(fireUnit.data), sin(fireUnit.data), 0.};
                auto tmp = baseCoordinate.directionWorldToBody(globalDirection);
                auto relativeDirection = coordinate.directionWorldToBody(tmp);
                expectDirection = atan2(relativeDirection.y, relativeDirection.x);
                expectPitch = 0;
            } else {
                auto it = scannedMemory.value().find(fireUnit.data);
                my_assert(it != scannedMemory.value().end() && isTargetAvailable(it->second));
                // TODO: cache?
                auto &info = std::get<1>(it->second);
                auto tmp = baseCoordinate.positionWorldToBody(info.position + lagFrameCounter * dt * info.velocity);
                auto relativePosition = coordinate.positionWorldToBody(tmp);
                if (relativePosition.norm() < fireUnit.weapon.range) {
                    inRange = true;
                }
                if (relativePosition.norm() == 0) {
                    expectPitch = expectDirection = 0;
                } else {
                    expectDirection = atan2(relativePosition.y, relativePosition.x);
                    expectPitch = asin(relativePosition.z / relativePosition.norm());
                }
            }

            double rotateYaw = angleNormalize(expectDirection - fireUnit.presentDirection.yaw);
            double rotatePitch = angleNormalize(expectPitch - fireUnit.presentDirection.pitch);
            if (fabs(rotateYaw) < fireUnit.rotateSpeed.yaw * dt &&
                fabs(rotatePitch) < fireUnit.rotateSpeed.pitch * dt) {
                // 4. check if aimed and target in range
                fireUnit.presentDirection.yaw = expectDirection;
                fireUnit.presentDirection.pitch = expectPitch;
                if (inRange) {
                    ret.emplace(fireUnit);
                }
            } else {
                fireUnit.presentDirection.yaw += fireUnit.rotateSpeed.yaw * dt * (signbit(rotateYaw) ? -1 : 1);
                fireUnit.presentDirection.pitch += fireUnit.rotateSpeed.pitch * dt * (signbit(rotatePitch) ? -1 : 1);
                fireUnit.presentDirection.yaw = angleNormalize(fireUnit.presentDirection.yaw);
            }
        }
        return ret;
    }
    void setFire(double dt, Components &c, const std::set<std::reference_wrapper<FireUnit>> &aimedAndInRange) {
        auto &selfPosition = c.getSpecificSingleton<Coordinate>().value().position;
        auto &mem = c.getSpecificSingleton<ScannedMemory>().value();
        auto &fireEvents = c.getSpecificSingleton<EventBuffer>().value();
        for (auto &&[id, fireUnit] : c.getNormal<FireUnit>()) {
            if (!aimedAndInRange.contains(fireUnit) ||
                (fireUnit.state != FIRE_UNIT_STATE::SINGLE_SHOOT && fireUnit.state != FIRE_UNIT_STATE::MULTI_SHOOT) ||
                fireUnit.weapon.reloadingState != 0 || fireUnit.weapon.ammoRemain == 0) {
                continue;
            }
            fireEvents.emplace("FireDataOut",
                               weaponShoot(fireUnit, selfPosition, std::get<1>(mem[fireUnit.data]).position));
            // 1 shoot per frame
            break;
        }
    }
    bool isTargetAvailable(const std::tuple<double, carphymodel::EntityInfo> &tar) {
        return std::get<0>(tar) == 0. && std::get<1>(tar).baseInfo.damageLevel != DAMAGE_LEVEL::KK;
    }

    double angleNormalize(double angle) {
        if (angle > PI) {
            return angle - 2 * PI;
        }
        if (angle < -PI) {
            return angle + 2 * PI;
        }
        return angle;
    }

    FireEvent weaponShoot(carphymodel::FireUnit &fireUnit, const carphymodel::Vector3 &selfPosition,
                          const carphymodel::Vector3 &targetPosition) {
        fireUnit.weapon.ammoRemain -= 1;
        fireUnit.weapon.reloadingState = fireUnit.weapon.reloadingTime;
        if (fireUnit.state == FIRE_UNIT_STATE::SINGLE_SHOOT) {
            fireUnit.state = FIRE_UNIT_STATE::FREE;
        }
        return FireEvent{
            .weaponName = fireUnit.weapon.ammoType,
            .target = targetPosition,
            .position = selfPosition,
            .velocity = fireUnit.weapon.speed * (targetPosition - selfPosition).normalize(),
            .range = (targetPosition - selfPosition).norm(),
        };
    }
};

} // namespace carphymodel