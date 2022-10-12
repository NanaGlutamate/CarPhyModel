#include "cardamage.h"

namespace{

// 重力加速度
const double G = 9.8;
// PI
const double PI = 3.1415926535897932384626433;
// 无穷小量
const double INF_SMALL = 1e-6;
// 无穷大量
const double INF_BIG = 1e8;

};

void DamageSystem::tick(
    double dt, 
    DamageList& damage, 
    const Coordinate& baseCoordinate, 
    const CoordinateList& coordinate, 
    const FireEvent& fireEvent
){
    auto ammunitionModel = adf->getAmmunitionDamage(fireEvent.weaponName);
    auto local_p = baseCoordinate.positionWorldToBody(fireEvent.position);
    auto local_d = baseCoordinate.directionWorldToBody(fireEvent.direction);
    auto local_v = baseCoordinate.directionWorldToBody(fireEvent.velocity);

    for(auto&& [id, _damage_model, _coordinate] : make_entity(damage, coordinate)){
        ammunitionModel->updateDamage(_damage_model, _coordinate, local_p, local_d, local_v, fireEvent.range);
    }
    
    return;
};