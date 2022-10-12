#include "HE.h"

namespace{

// 重力加速度
constexpr const double G = 9.8;
// PI
constexpr const double PI = 3.1415926535897932384626433;
// 无穷小量
constexpr const double INF_SMALL = 1e-10;
// 无穷大量
constexpr const double INF_BIG = 1e10;

// 计算毁伤碰撞盒与球距离
double collision(const Vector3& pos, const PartDamageModel& pdm, const Coordinate& coordinate){
    using std::pow;
    using std::sqrt;
    const Vector3 pos_local = coordinate.positionWorldToBody(pos);
    double r2 = 0.;
    for(size_t i=0; i<3; ++i){
        if(pos_local[i] < -pdm.size[i])r2 += pow(pos_local[i] + pdm.size[i], 2);
        if(pos_local[i] > pdm.size[i])r2 += pow(pos_local[i] - pdm.size[i], 2);
    }
    return sqrt(r2);
};

}

void HEDamage::updateDamage(PartDamageModel& pdm, const Coordinate& coordinate, const Vector3& pos, const Vector3& dir, const Vector3& vel, double range) const{
    auto radius = collision(pos, pdm, coordinate);
    if(radius <= damageTable[0]){
        pdm.damageLevel = DAMAGE_LEVEL::KK;
    }else if(radius <= damageTable[1]){
        if(pdm.damageLevel > DAMAGE_LEVEL::K)pdm.damageLevel = DAMAGE_LEVEL::K;
    }else if(radius <= damageTable[2]){
        if(pdm.damageLevel > DAMAGE_LEVEL::M)pdm.damageLevel = DAMAGE_LEVEL::M;
    }
};