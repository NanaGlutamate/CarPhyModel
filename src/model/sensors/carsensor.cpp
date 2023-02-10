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
    for(auto& x : c.getSpecificSingletonComponent<ScannedMemory>().value()){
        std::get<0>(x.second) += dt;
    }
    for(auto&& [id, _sensor] : c.getNormalComponents<SensorData>()){
        if(auto _damage = c.getSpecificNormalComponent<DamageModel>(id); _damage.has_value()){
            DamageModel& damage = _damage.value();
            if(damage.damageLevel == DAMAGE_LEVEL::K || damage.damageLevel == DAMAGE_LEVEL::KK){
                continue;
            }
        }
        auto sensor = SensorFactory::getProduct(_sensor.type).lock();
        auto& baseCoordinate = c.getSpecificSingletonComponent<Coordinate>().value();
        auto& hull = c.getSpecificSingletonComponent<Hull>().value();
        for(auto&& [vid, _entityInfo] : c.getSpecificSingletonComponent<ScannedMemory>().value()){
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