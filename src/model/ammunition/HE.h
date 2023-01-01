#pragma once

#include "ammunition.hpp"

namespace carPhyModel{

// HE
class HEDamage : public AmmunitionDamage{
    std::array<double, 3> damageTable;
public:
    //! @param damage_table: {KK range, K range, M range}
    HEDamage(const std::array<double, 3>& damageTable): damageTable(damageTable){};
    virtual void updateDamage(PartDamageModel& pdm, const Block& size, const Coordinate& coordinate, const Vector3& pos, const Vector3& dir, const Vector3& vel, double range) const override;
};

};