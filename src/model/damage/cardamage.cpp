#include "cardamage.h"
#include "../ammunition/ammunitionfactory.hpp"

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

namespace carPhyModel{

void DamageSystem::tick(double dt, Components& c){

    auto&& baseCoordinate = std::get<0>(c.getSingletonComponent<Coordinate>());

    for(auto&& fireEvent : std::get<0>(c.getSingletonComponent<FireEventQueue>())){
        auto ammunitionModel = AmmunitionDamageFactory::getProduct(fireEvent.weaponName).lock();
        auto local_p = baseCoordinate.positionWorldToBody(fireEvent.position);
        auto local_d = baseCoordinate.directionWorldToBody(fireEvent.direction);
        auto local_v = baseCoordinate.directionWorldToBody(fireEvent.velocity);

        for(auto&& [id, _damage_model, _coordinate] : c.getNormalComponent<PartDamageModel, Coordinate>()){
            ammunitionModel->updateDamage(_damage_model, _coordinate, local_p, local_d, local_v, fireEvent.range);
        }
    }
    return;
};

};