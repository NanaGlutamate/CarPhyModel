#pragma once

#include <cmath>
#include "../../tools/datastructure.hpp"
#include "../../tools/initjob.hpp"
#include "../../environment.h"
#include "./extern/main.h"

namespace carphymodel{

class TrackMoveSystem{
private:
    EnvironmentInfoAgent env;
    void updateState(double dt, Coordinate& baseCoordinate, Hull& hull, WheelMotionParamList& params);
public:
    TrackMoveSystem():env(){};

    //! @param dt: 上一次调用后的时间
    //! @param baseCoordinate: 车辆随体坐标系
    //! @param hull: 运动参数：速度、角速度、质量、转动惯量
    //! @param exp_direction: 期望方向
    //! @param exp_speed: 期望速度
    void tick(
        double dt, 
        Coordinate& baseCoordinate, 
        Hull& hull, 
        const Vector3& exp_direction, 
        double exp_speed
    ){
        using namespace externModel::track;
        static SixDof sixDOF;
        static DataStore param;
        static DoJobOnConstruct init([&](){
            param.init();
        });
    }
};

}