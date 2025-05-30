﻿#pragma once

#include <cmath>
#include "../../tools/datastructure.hpp"
#include "../../environment.h"

namespace carphymodel{

// 轮式车辆运动系统
class WheelMoveSystem{
private:
    inline static EnvironmentInfoAgent env{};
    // void updateState(double dt, Coordinate& baseCoordinate, Hull& hull, WheelMotionParamList& params);
public:
    WheelMoveSystem() = default;

    //! @param dt: 上一次调用后的时间
    //! @param baseCoordinate: 车辆随体坐标系
    //! @param hull: 运动参数：速度、角速度、质量、转动惯量
    //! @param expectYaw: 期望方向
    //! @param expectSpeed: 期望速度
    //! @param params: 车辆运动参数
    static void tick(
        double dt, 
        Coordinate& baseCoordinate, 
        Hull& hull, 
        double expectYaw, 
        double expectSpeed, 
        WheelMotionParamList& params
    );

    static void directTick(
        double dt, 
        Coordinate& baseCoordinate, 
        Hull& hull, 
        double expectYaw, 
        double expectSpeed, 
        WheelMotionParamList& params
    ){};
};

}