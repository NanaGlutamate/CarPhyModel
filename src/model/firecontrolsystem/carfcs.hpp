#pragma once

#include <any>
#include <functional>
#include <set>
#include <tuple>

#include "../framework/system.hpp"
#include "../tools/datastructure.hpp"
#include "../tools/myassert.hpp"

namespace carphymodel {

class FireControlSystem : public System {
  public:
    FireControlSystem() = default;
    virtual void tick(double dt, Components &c) override {
        using namespace command;
        using namespace std;
        const static map<COMMAND_TYPE, FIRE_UNIT_STATE> commands{
            {COMMAND_TYPE::SHOOT, FIRE_UNIT_STATE::SINGLE_SHOOT},
            {COMMAND_TYPE::LOCK_DIRECTION, FIRE_UNIT_STATE::LOCK_DIRECTION},
            {COMMAND_TYPE::LOCK_TARGET, FIRE_UNIT_STATE::LOCK_TARGET},
            {COMMAND_TYPE::UNLOCK, FIRE_UNIT_STATE::FREE},
        };
        for (auto &&[k, v] : c.getSpecificSingleton<InputBuffer>().value()) {
            if (commands.contains(k)) {
                auto [index, param] = any_cast<tuple<double, double>>(v);
                auto &tmp = getNthFireUnit((size_t)index, c);
                tmp.state = commands.find(k)->second;
                tmp.data = param;
            } else if (k == COMMAND_TYPE::FREE_SHOOT || k == COMMAND_TYPE::STOP_SHOOT) {
                auto state = (k == COMMAND_TYPE::FREE_SHOOT) ? FIRE_UNIT_STATE::MULTI_SHOOT
                                                             : FIRE_UNIT_STATE::FREE;
                for (auto &&[_id, _fireUnit] : c.getNormal<FireUnit>()) {
                    _fireUnit.state = state;
                }
                break;
            }
        }
        const auto selfPosition = c.getSpecificSingleton<Coordinate>()->position;
        for (auto &&[_id, _damageModel, _fireUnit] : c.getNormal<DamageModel, FireUnit>()) {
            if (_damageModel.damageLevel != DAMAGE_LEVEL::N ||
                _damageModel.damageLevel != DAMAGE_LEVEL::M)
                continue;
            auto &&info = c.getSpecificSingleton<ScannedMemory>().value();
            updateWeaponReloading(_fireUnit, dt);
            // TODO: MULTI_SHOOT with no target avaliable
            if (isWeaponFireable(_fireUnit) &&
                isTargetAvailable(_fireUnit, info, selfPosition)) {
                const auto targetPosition =
                    get<1>(info.find(static_cast<size_t>(_fireUnit.data))->second).position;
                c.getSpecificSingleton<OutputBuffer>()->emplace(
                    "FireDataOut", weaponShoot(_fireUnit, selfPosition, targetPosition));
            } else if (_fireUnit.state == FIRE_UNIT_STATE::FREE) {
                continue;
            }
            // TODO: rotate and lock
        }
    };
    virtual ~FireControlSystem() = default;

  private:
    FireUnit &getNthFireUnit(size_t n, Components &c) {
        size_t cnt = 0;
        for (auto &&[_id, _fireUnit] : c.getNormal<FireUnit>()) {
            if (cnt == n) {
                return _fireUnit;
            }
            ++cnt;
        }
        my_assert(false, "Invalid Weapon Index " + std::to_string(n));
        return get<1>(*c.getNormal<FireUnit>().begin());
    }
    bool isTargetAvailable(carphymodel::FireUnit &_fireUnit, carphymodel::ScannedMemory &info,
                           const Vector3 &selfPosition) {
        auto targetID = static_cast<size_t>(_fireUnit.data);
        auto it = info.find(targetID);
        return it != info.end() && get<0>(it->second) == 0. &&
               get<1>(it->second).baseInfo.damageLevel != DAMAGE_LEVEL::KK &&
               (get<1>(it->second).position - selfPosition).norm() < _fireUnit.weapon.range;
    }
    bool isWeaponFireable(carphymodel::FireUnit &_fireUnit) {
        return (_fireUnit.state == FIRE_UNIT_STATE::MULTI_SHOOT ||
                _fireUnit.state == FIRE_UNIT_STATE::SINGLE_SHOOT) &&
               _fireUnit.weapon.reloadingState == 0. && _fireUnit.weapon.ammoRemain != 0;
    }
    void updateWeaponReloading(carphymodel::FireUnit &_fireUnit, double dt) {
        _fireUnit.weapon.reloadingState -= dt;
        if (_fireUnit.weapon.reloadingState <= 0.) {
            _fireUnit.weapon.reloadingState = 0;
        }
    }
    FireEvent weaponShoot(carphymodel::FireUnit &_fireUnit,
                          const carphymodel::Vector3 &selfPosition,
                          const carphymodel::Vector3 &targetPosition) {
        _fireUnit.weapon.ammoRemain -= 1;
        if (_fireUnit.state == FIRE_UNIT_STATE::SINGLE_SHOOT) {
            _fireUnit.state = FIRE_UNIT_STATE::FREE;
        }
        return FireEvent{
            .weaponName = _fireUnit.weapon.ammoType,
            .target = targetPosition,
            .position = selfPosition,
            .velocity = _fireUnit.weapon.speed * (targetPosition - selfPosition).normalize(),
            .range = (targetPosition - selfPosition).norm(),
        };
    }
    // void shoot(FireUnit& f, Components& c){}
};

} // namespace carphymodel