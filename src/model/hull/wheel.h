#pragma once

#include <cmath>
#include "../tools/datastructure.hpp"
#include "../environment.h"

namespace carPhyModel{

// 轮式车辆运动系统
class CarHullSystem{
private:
    EnvironmentInfoAgent env;
    void updateState(double dt, Coordinate& baseCoordinate, Hull& hull, WheelMotionParamList& params);
public:
    CarHullSystem():env(){};

    //! @param dt: 上一次调用后的时间
    //! @param baseCoordinate: 车辆随体坐标系
    //! @param hull: 运动参数：速度、角速度、质量、转动惯量
    //! @param exp_direction: 期望方向
    //! @param exp_speed: 期望速度
    //! @param params: 车辆运动参数
    void tick(
        double dt, 
        Coordinate& baseCoordinate, 
        Hull& hull, 
        const Vector3& exp_direction, 
        double exp_speed, 
        WheelMotionParamList& params
    );

    // 最小运动限制下的tick
    // 未完成
    void unconstrainedTick(
        double dt, 
        Coordinate& baseCoordinate, 
        Hull& hull, 
        double wheel_angle, 
        double acceleration, 
        WheelMotionParamList& params
    );
};

};