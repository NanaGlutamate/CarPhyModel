#ifndef __SRC_MODEL_AMMUNITIONS_HE_H__
#define __SRC_MODEL_AMMUNITIONS_HE_H__

#include "ammunition.hpp"

// HE
class HEDamage : public AmmunitionDamage{
    std::array<double, 3> damageTable;
public:
    //! @param damage_table: {KK range, K range, M range}
    HEDamage(std::array<double, 3> damageTable): damageTable(damageTable){};
    virtual void updateDamage(PartDamageModel& pdm, const Coordinate& coordinate, const Vector3& pos, const Vector3& dir, const Vector3& vel, double range) const override;
};

#endif