#include "AP.h"
#include "../tools/constant.hpp"

namespace{
    
using namespace carphymodel;

// array collision with AABB
std::tuple<double, bool> collision(
    const Vector3& dir,
    const Vector3& pos,
    const Block& size,
    const Coordinate& coordinate)
{
    using std::fabs;
    using std::fmax;
    using std::fmin;
    const Vector3 dir_local = coordinate.directionWorldToBody(dir);
    const Vector3 pos_local = coordinate.positionWorldToBody(pos);
    double depth_min = -INF_BIG, depth_max = INF_BIG;
    for(size_t i=0; i<3; ++i){
        if(fabs(dir_local[i]) < INF_SMALL){
            if(pos_local[i] < -size[i] / 2. || pos_local[i] > size[i] / 2.){return std::make_tuple(0., false);}
        }else{
            const double d1 = (size[i] / 2. - pos_local[i]) / dir_local[i];
            const double d2 = (-size[i] / 2. - pos_local[i]) / dir_local[i];
            depth_min = fmax(depth_min, fmin(d1, d2));
            depth_max = fmin(depth_max, fmax(d1, d2));
        }
    }
    return std::make_tuple(depth_min, depth_max > depth_min);
}

void updateDamage(DAMAGE_LEVEL ){}

};

namespace carphymodel{

void APDamage::updateDamage(
    DamageModel& pdm, 
    const Block& size,
    const Coordinate& coordinate, 
    const Vector3& pos, 
    const Vector3& dir, 
    const Vector3& vel, 
    double range) const{
    auto [depth, is_hit] = collision(vel, pos, size, coordinate);
    if(is_hit){
        // TODO: calculate speed with look-up-table
        const double speed = vel.norm();
        // calculate damage with look-up-table
        if(speed >= damageTable[0]){
            pdm.damageLevel = DAMAGE_LEVEL::KK;
        }else if(speed >= damageTable[1]){
            if(pdm.damageLevel < DAMAGE_LEVEL::K)pdm.damageLevel = DAMAGE_LEVEL::K;
        }else if(speed >= damageTable[2]){
            if(pdm.damageLevel < DAMAGE_LEVEL::M)pdm.damageLevel = DAMAGE_LEVEL::M;
        }
    }
    return;
};

};