#include "HE.h"
#include "../tools/constant.hpp"

namespace{
    
using namespace carphymodel;

// 计算毁伤碰撞盒与球距离
double collision(
    const Vector3& pos, 
    const Block& size, 
    const Coordinate& coordinate){
    const Vector3 pos_local = coordinate.positionWorldToBody(pos);
    double r2 = 0.;
    for(size_t i=0; i<3; ++i){
        if(pos_local[i] < -size[i] / 2.)r2 += pow(pos_local[i] + size[i] / 2., 2);
        if(pos_local[i] > size[i] / 2.)r2 += pow(pos_local[i] - size[i] / 2., 2);
    }
    return sqrt(r2);
}

}

namespace carphymodel{

void HEDamage::updateDamage(
    DamageModel& pdm, 
    const Block& size,
    const Coordinate& coordinate, 
    const Vector3& pos, 
    const Vector3& dir, 
    const Vector3& vel, 
    double range) const{
    auto radius = collision(pos, size, coordinate);
    if(radius <= damageTable[0]){
        pdm.damageLevel = DAMAGE_LEVEL::KK;
    }else if(radius <= damageTable[1]){
        if(pdm.damageLevel < DAMAGE_LEVEL::K)pdm.damageLevel = DAMAGE_LEVEL::K;
    }else if(radius <= damageTable[2]){
        if(pdm.damageLevel < DAMAGE_LEVEL::M)pdm.damageLevel = DAMAGE_LEVEL::M;
    }
};

};