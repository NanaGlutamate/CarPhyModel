#pragma once

#include "../tools/datastructure.hpp"
#include "ammunition.hpp"

namespace carphymodel{

// AP
class APDamage final : public AmmunitionDamage{
public:
    /// @brief damage table, {KK range, K range, M range}
    std::array<double, 3> damageTable;
    /// @brief piercing ability of current ammunition, meter
    double piercingAbility;
    
    // virtual void updateDamage(DamageModel& pdm, const Block& size, const Coordinate& coordinate, const Vector3& pos, const Vector3& dir, const Vector3& vel, double range) const override;
    virtual void updateDamage(const FireEvent& fireEvent, Components& c) const override;
};

};