#pragma once

#include <any>
#include <functional>
#include <set>
#include <tuple>

#include "../tools/myassert.hpp"
#include "../framework/system.hpp"
#include "../tools/datastructure.hpp"

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
        for (auto &&[_id, _fireUnit] : c.getNormal<FireUnit>()) {
            auto &&info = c.getSpecificSingleton<ScannedMemory>().value();
            _fireUnit.weapon.reloadingState -= dt;
            if (_fireUnit.weapon.reloadingState <= 0.) {
                _fireUnit.weapon.reloadingState = 0;
            }
            // TODO: rotate speed
            if ((_fireUnit.state == FIRE_UNIT_STATE::MULTI_SHOOT ||
                 _fireUnit.state == FIRE_UNIT_STATE::SINGLE_SHOOT) &&
                _fireUnit.weapon.reloadingState == 0. && _fireUnit.weapon.ammoRemain != 0 &&
                targetAvailable(static_cast<size_t>(_fireUnit.data), info)) {
                const auto targetPosition = get<1>(info.find(static_cast<size_t>(_fireUnit.data))->second).position;
                const auto selfPosition = c.getSpecificSingleton<Coordinate>()->position;
                _fireUnit.weapon.ammoRemain -= 1;
                c.getSpecificSingleton<OutputBuffer>()->emplace(
                    "FireDataOut", FireEvent{
                                       .weaponName = _fireUnit.weapon.ammoType,
                                       .target = targetPosition,
                                       .position = selfPosition,
                                       .velocity = _fireUnit.weapon.speed * (targetPosition - selfPosition).normalize(),
                                       .range = (targetPosition - selfPosition).norm(),
                                   });
                if (_fireUnit.state == FIRE_UNIT_STATE::SINGLE_SHOOT) {
                    _fireUnit.state = FIRE_UNIT_STATE::FREE;
                }
            } else if (_fireUnit.state == FIRE_UNIT_STATE::FREE) {
                continue;
            }
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
    };
    bool targetAvailable(size_t targetID, carphymodel::ScannedMemory &info) {
        auto it = info.find(targetID);
        return it != info.end() && get<0>(it->second) == 0. &&
               get<1>(it->second).baseInfo.damageLevel != DAMAGE_LEVEL::KK;
    }
    // void shoot(FireUnit& f, Components& c){}
};

} // namespace carphymodel