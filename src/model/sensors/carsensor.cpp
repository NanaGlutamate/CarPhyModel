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
    // auto factory = SensorFactory::getInstance();
    // for(auto& x : scannedMemory){
    //     x.second.lastScanned += dt;
    // }
    // for(auto&& [id, _sensor, _damage] : make_entity(sensorData, damage)){
    //     if(_damage.damageLevel == DAMAGE_LEVEL::K || _damage.damageLevel == DAMAGE_LEVEL::KK)continue;
    //     auto sensor = factory->getSensorModel(_sensor.type);
    //     for(auto& [vid, _entityInfo] : scannedMemory){
    //         if (_entityInfo.lastScanned != 0. && sensor->isDetectable(baseCoordinate, _entityInfo, _sensor, hull)) {
    //             _entityInfo.lastScanned = 0.;
    //         }
    //     }
    // }
    // return;
};

}