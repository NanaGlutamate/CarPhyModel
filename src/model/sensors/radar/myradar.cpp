#include <mutex>
#include <iostream>

#include "myradar.h"
#include "extern/Radar.h"

namespace {
//using namespace radar;
std::mutex mtx;

struct DoJobOnConstruct{
    template<typename Job, typename ...Params>
    DoJobOnConstruct(Job&& j, Params&& ...params){std::invoke(std::forward<Job>(j), std::forward<Params>(params)...);};
};

}

namespace carphymodel{

bool MyRadar::isDetectable(const Coordinate& self, const EntityInfo& e, const SensorData& sensor, const Hull& hull, double jammerPower) const
{
    static externModel::radar::Radar PdRadar1;
    static DoJobOnConstruct init{[&](){
        PdRadar1.Init();
        PdRadar1.Set_JamPower(0);
    }};
    std::lock_guard lock{mtx};
    PdRadar1.ENURadarState.rPos = { self.position.x, self.position.y, self.position.z };
    PdRadar1.ENURadarState.rVel = { hull.velocity.x, hull.velocity.y, hull.velocity.z };
    PdRadar1.ENUTargetState.tPos = { e.position.x, e.position.y, e.position.z };
    PdRadar1.ENUTargetState.tVel = { e.velocity.x, e.velocity.y, e.velocity.z };
    PdRadar1.Set_JamPower(jammerPower);
    PdRadar1.update();
    return PdRadar1.ENUTargetState.detected;
}

};