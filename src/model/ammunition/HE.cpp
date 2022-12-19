#include "HE.h"
#include "../tools/constant.hpp"

namespace{
    
using namespace carPhyModel;

// 计算毁伤碰撞盒与球距离
double collision(const carPhyModel::Vector3& pos, const carPhyModel::PartDamageModel& pdm, const carPhyModel::Coordinate& coordinate){
    using std::pow;
    using std::sqrt;
    const carPhyModel::Vector3 pos_local = coordinate.positionWorldToBody(pos);
    double r2 = 0.;
    for(size_t i=0; i<3; ++i){
        if(pos_local[i] < -pdm.size[i] / 2.)r2 += pow(pos_local[i] + pdm.size[i] / 2., 2);
        if(pos_local[i] > pdm.size[i] / 2.)r2 += pow(pos_local[i] - pdm.size[i] / 2., 2);
    }
    return sqrt(r2);
};

}

namespace carPhyModel{

void HEDamage::updateDamage(PartDamageModel& pdm, const Coordinate& coordinate, const Vector3& pos, const Vector3& dir, const Vector3& vel, double range) const{
    auto radius = collision(pos, pdm, coordinate);
    if(radius <= damageTable[0]){
        pdm.damageLevel = DAMAGE_LEVEL::KK;
    }else if(radius <= damageTable[1]){
        if(pdm.damageLevel < DAMAGE_LEVEL::K)pdm.damageLevel = DAMAGE_LEVEL::K;
    }else if(radius <= damageTable[2]){
        if(pdm.damageLevel < DAMAGE_LEVEL::M)pdm.damageLevel = DAMAGE_LEVEL::M;
    }
};

};