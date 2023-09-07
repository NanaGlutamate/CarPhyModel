#include "myantenna.h"
#include "extern/Comm.h"
#include "../../tools/initjob.hpp"

namespace carphymodel{

bool MyAntenna::sendMessage(const Vector3& self, const Vector3& target){
    using namespace externModel::comm;
	static Comm comm1, comm2;
    static Env environment{1, 1, 1};
    static carrier plat{0., 0., 0.};
	// static facilityPOS pos1;
    static DoJobOnConstruct init{[&](){
        comm1.Init();
        comm2.Init();
        comm1.Comm_Init(&comm2);
        comm2.Comm_Init(&comm1);
    }};
	facilityPOS pos1, pos2;
    // TODO:
	pos1.Comm_Set_longitude = target.y;
    pos1.Comm_Set_latitude = target.x;
    pos1.Comm_Set_altitude = -target.z;
	pos2.Comm_Set_longitude = self.y;
	pos2.Comm_Set_latitude = self.x;
	pos2.Comm_Set_altitude = -self.z;
    double jam = 1e-16;
	//一号设备
	comm1.SetInput(pos1, environment, jam, plat);
	//二号设备
	comm2.SetInput(pos2, environment, jam, plat);
	comm1.Tick(0);
	comm2.Tick(0);
	//cout << '\t' << ber << '\n' << endl;
	bool connect = comm2.Getconnect();
	return connect;
}

}