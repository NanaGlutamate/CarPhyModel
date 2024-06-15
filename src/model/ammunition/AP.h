#pragma once

#include <map>

#include "../tools/datastructure.hpp"
#include "ammunition.hpp"

namespace carphymodel{

// AP
class APDamage final : public AmmunitionDamage{
public:
    /// @brief piercing ability table, range -> piercing ability
    std::map<double, double> piercingAbilityTable;
    /// @brief piercing ability of current ammunition, meter
    // double piercingAbility;
    
    APDamage(std::map<double, double> table) : piercingAbilityTable(std::move(table)) {}
    // virtual void updateDamage(DamageModel& pdm, const Block& size, const Coordinate& coordinate, const Vector3& pos, const Vector3& dir, const Vector3& vel, double range) const override;
    virtual void updateDamage(const FireEvent& fireEvent, Components& c) const override;
};

};