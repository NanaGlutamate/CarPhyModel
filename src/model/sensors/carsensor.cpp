#include "carsensor.h"

#include <algorithm>
#include <tuple>
#include "../tools/datastructure.hpp"
#include "../sensors/sensorfactory.hpp"

namespace{

};

namespace carphymodel{

void SensorSystem::tick(double dt, Components& c)
{
    auto& mem = c.getSpecificSingleton<ScannedMemory>().value();
    auto& baseCoordinate = c.getSpecificSingleton<Coordinate>().value();
    auto& hull = c.getSpecificSingleton<Hull>().value();
    for(auto& x : mem){
        std::get<0>(x.second) += dt;
    }
    for(auto&& [id, _sensor, _damage] : c.getNormal<SensorData, DamageModel>()){
        if(_damage.damageLevel == DAMAGE_LEVEL::K || _damage.damageLevel == DAMAGE_LEVEL::KK){
            continue;
        }
        auto sensor = SensorFactory::getProduct(_sensor.type);
        for(auto&& [vid, _entityInfo] : mem){
            if(std::get<0>(_entityInfo) != 0. && 
                sensor->isDetectable(baseCoordinate, std::get<1>(_entityInfo), _sensor, hull))
            {
                std::get<0>(_entityInfo) = 0.;
            }
        }
    }
    return;
}

}