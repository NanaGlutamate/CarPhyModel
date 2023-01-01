#include "carsensor.h"

#include <algorithm>
#include <tuple>
#include "../tools/datastructure.hpp"
#include "../sensors/sensorfactory.hpp"

namespace{

};

namespace carPhyModel{

void SensorSystem::tick(double dt, Components& c)
{
    for(auto& x : c.getSpecificSingletonComponent<ScannedMemory>().value()){
        x.second.lastScanned += dt;
    }
    for(auto&& [id, _sensor] : c.getNormalComponents<SensorData>()){
        if(auto _damage = c.getSpecificNormalComponent<PartDamageModel>(id); _damage.has_value()){
            PartDamageModel& damage = _damage.value();
            if(damage.damageLevel == DAMAGE_LEVEL::K || damage.damageLevel == DAMAGE_LEVEL::KK){
                continue;
            }
        }
        auto sensor = SensorFactory::getProduct(_sensor.type).lock();
        auto& baseCoordinate = c.getSpecificSingletonComponent<Coordinate>().value();
        auto& hull = c.getSpecificSingletonComponent<Hull>().value();
        for(auto&& [vid, _entityInfo] : c.getSpecificSingletonComponent<ScannedMemory>().value()){
            if(sensor->isDetectable(
                baseCoordinate,
                _entityInfo,
                _sensor,
                hull))
            {
                _entityInfo.lastScanned = 0.;
            }
        }
    }
    return;
}

}