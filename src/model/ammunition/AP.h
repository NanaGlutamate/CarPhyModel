#pragma once

#include "../tools/datastructure.hpp"
#include "ammunition.hpp"

namespace carphymodel{

// AP
class APDamage : public AmmunitionDamage{
    std::array<double, 3> damageTable;
    std::vector<std::tuple<double, double>> rangeSpeedTable;
public:
    //! @param damageTable: {KK range, K range, M range}
    //! @param rangeSpeedTable: {{range, speed}}, unused
    APDamage(const std::array<double, 3>& damageTable, const std::vector<std::tuple<double, double>>& rangeSpeedTable={}): damageTable(damageTable), rangeSpeedTable(rangeSpeedTable){};
    virtual void updateDamage(DamageModel& pdm, const Block& size, const Coordinate& coordinate, const Vector3& pos, const Vector3& dir, const Vector3& vel, double range) const override;
};

};