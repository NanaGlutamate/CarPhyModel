#include "myradar.h"
#include "extern/Radar.h"
#include <iostream>

namespace {
//using namespace radar;

struct DoJobOnConstruct{
    template<typename Job>
    DoJobOnConstruct(Job&& j){j();};
};

}

namespace carPhyModel{

bool MyRadar::isDetectable(const Coordinate& self, const EntityInfo& e, const SensorData& sensor, const Hull& hull) const
{
    static externModel::radar::Radar PdRadar1;
    static DoJobOnConstruct init{[&](){
        PdRadar1.Init();
        PdRadar1.Set_JamPower(0);
    }};
    externModel::radar::Vector3 rpos{ self.position.x, self.position.y, self.position.z }, rvel{ hull.velocity.x, hull.velocity.y, hull.velocity.z };
    externModel::radar::Vector3 tpos{ e.position.x, e.position.y, e.position.z }, tvel{ e.velocity.x, e.velocity.y, e.velocity.z };
    PdRadar1.InitValue("RPos", &rpos);
    PdRadar1.InitValue("RVel", &rvel);
    PdRadar1.SetInput({ {"Target_Position", tpos}, {"Target_Velocity", tvel} });
    PdRadar1.update();
    return PdRadar1.ENUTargetState.detected;
}

};