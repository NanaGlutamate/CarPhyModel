#pragma once

#include "ammunition.hpp"

namespace carphymodel{

// HE
class HEDamage : public AmmunitionDamage{
    std::array<double, 3> damageTable;
public:
    //! @param damage_table: {KK range, K range, M range}
    HEDamage(const std::array<double, 3>& damageTable): damageTable(damageTable){};
    virtual void updateDamage(DamageModel& pdm, const Block& size, const Coordinate& coordinate, const Vector3& pos, const Vector3& dir, const Vector3& vel, double range) const override;
    virtual void updateDamage(const FireEvent& fireEvent, const Components& c) const override;
};

};