#include "myantenna.h"
#include "extern/Comm.h"

namespace{

struct DoJobOnConstruct{
    template<typename Job, typename ...Params>
    DoJobOnConstruct(Job&& j, Params&& ...params){std::invoke(std::forward<Job>(j), std::forward<Params>(params)...);};
};

}

namespace carPhyModel{

bool MyAntenna::sendMessage(){
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
	// pos1.Comm_Set_longitude = Posa_x;
	// pos1.Comm_Set_latitude = Posa_y;
	// pos1.Comm_Set_altitude = Posa_z;
	// pos2.Comm_Set_longitude = Posb_x;
	// pos2.Comm_Set_latitude = Posb_y;
	// pos2.Comm_Set_altitude = Posb_z;
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