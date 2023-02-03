#include "wheel.h"
#include "../../tools/constant.hpp"

namespace{

using namespace carphymodel;

// 投射到给定范围内
double clamp(double value, double min, double max){
    if(value < min) value = min;
    if(value > max) value = max;
    return value;
};

double clamp(double value, double bound){
    if(value < -bound) value = -bound;
    if(value > bound) value = bound;
    return value;
};

double softClamp(double value, double bound){
    return atan(value * (PI/2.) / bound) / (PI/2.) * bound;
};

// 获取当前转角时角速度与速度比例
double angularLinearRatio(double rad, double LENGTH){
    return tan(rad) / LENGTH;
};

// 获取符号
int sign(double value){
    if(value > INF_SMALL) return 1;
    if(value < -INF_SMALL) return -1;
    return 0;
};

};

namespace carphymodel{

// 采用前右下坐标系，速度和转角分别控制以支持原地转方向盘、缓慢转弯、行进中变向等操作
// 由于动力学之上会采用控制器，无需考虑显式欧拉法的不稳定问题，出于简单考虑直接使用显式欧拉法
// 影响因素：
// 前轮转向：1. 最大转角 2. 最大转动速度 3. 最大侧向加速度
// 速度：1. 最大直线速度 2. 最大减速加速度 3. 最大侧向加速度 4. 最大前进加速度 5. 坡度带来的重力加速度分量
// 由于前轮转向需要时间，故速度也需要受最大侧向加速度约束
// 控制方法：符合上述约束的条件下尽可能快速地向目标调整
// TODO：倒车
void WheelMoveSystem::tick(double dt, Coordinate& baseCoordinate, Hull& hull, const Vector3& exp_direction, double exp_speed, WheelMotionParamList& params){
    using std::atan2;
    using std::tan;
    using std::fabs;
    using std::fmin;
    using std::sqrt;
    using std::sin;
    using std::cos;
    using val = const double;
    using var = double;

    const Vector3 front_direction = baseCoordinate.directionBodyToWorld(Vector3(1., 0., 0.));
    const Vector3 local_exp_direction = baseCoordinate.directionWorldToBody(exp_direction);
    val speed = hull.velocity.norm();
    
    // 获得当前坡度，得到局部的一维运动环境参数
    val slope = env.getSlope(baseCoordinate.position, front_direction);

    // 重力向前加速度分量
    val gravity_acceleration = -G * slope / sqrt(1 + slope*slope);
    // 最大直线速度、最大侧向加速度约束下的最大速度
    val max_speed = (fabs(params.radius) < INF_SMALL)?params.MAX_LINEAR_SPEED :
                                                      fmin(params.MAX_LINEAR_SPEED, sqrt(params.MAX_LATERAL_ACCELERATION * params.LENGTH / fabs(tan(params.radius))));
    // 最大直线速度、最大侧向加速度约束下的期望速度
    val aviliable_exp_speed = clamp(exp_speed, max_speed);
    // 期望速度与当前速度的差，目标偏快为正
    val speed_diff = aviliable_exp_speed - speed;
    // 速度变化量
    val dv = clamp(speed_diff, (gravity_acceleration - params.MAX_BRAKE_ACCELERATION) * dt, (gravity_acceleration + params.MAX_FRONT_ACCELERATION) * dt);

    // 目标偏航角与当前偏航角的差，目标偏右为正
    val exp_yaw_diff = atan2(local_exp_direction.y, local_exp_direction.x);
    // 矫正目标车轮朝向，避免转弯加速度过大
    val aviliable_angle = (speed < INF_SMALL)?PI/2:atan(params.MAX_LATERAL_ACCELERATION * params.LENGTH / (speed * speed));
    // 最大转角与最大侧向加速度约束下目标转角
    val exp_radius = clamp(params.radius + exp_yaw_diff, fmin(params.MAX_ANGLE, aviliable_angle));
    // 转动速度约束下可行转动量
    val delta_radius = clamp(exp_radius - params.radius, params.ROTATE_SPEED * dt);
    val wheel_angle = delta_radius;

    params.radius = wheel_angle;

    // 计算新速度和角速度
    val new_speed = clamp(speed + dv, fmin(params.MAX_LINEAR_SPEED * cos(params.radius), max_speed));
    // 角速度只与速度和前轮转角有关
    val new_angular_speed = new_speed * angularLinearRatio(params.radius, params.LENGTH);

    val angular_speed = hull.palstance.norm() * sign(hull.palstance.z);
    // 计算新朝向
    val dyaw = angular_speed * dt;
    const Vector3 new_front_direction = baseCoordinate.directionBodyToWorld(Vector3(cos(dyaw), sin(dyaw), 0.));
    
    // 计算新位置
    const Vector3 new_position_unlanded = baseCoordinate.position + hull.velocity * dt;
    // 投影到地面
    const Vector3 new_position = {new_position_unlanded.x, new_position_unlanded.y, -env.getAltitude(new_position_unlanded)};

    // 计算新随体坐标系姿态四元数
    val new_yaw = atan2(new_front_direction.y, new_front_direction.x);
    val new_pitch = atan(env.getSlope(new_position, new_front_direction));
    val new_roll = atan(env.getSlope(new_position, new_front_direction.out({0., 0., -1.})));
    const Quaternion new_altitude(new_roll, new_pitch, new_yaw);

    // 更新坐标系
    baseCoordinate.position = new_position;
    baseCoordinate.altitude = {new_altitude.x, new_altitude.y, new_altitude.z};

    // 更新速度和角速度
    hull.velocity = baseCoordinate.directionBodyToWorld(Vector3(new_speed, 0., 0.));
    hull.palstance = baseCoordinate.directionBodyToWorld(Vector3(0., 0., new_angular_speed));

    return;
};

// void WheelMoveSystem::unconstrainedTick(double dt, Coordinate& baseCoordinate, Hull& hull, double wheel_angle, double acceleration, WheelMotionParamList& params){
//     using std::atan2;
//     using std::tan;
//     using std::fabs;
//     using std::fmin;
//     using std::sqrt;
//     using std::sin;
//     using std::cos;
//     using val = const double;
//     using var = double;
// };

// void WheelMoveSystem::updateState(double dt, Coordinate& baseCoordinate, Hull& hull, WheelMotionParamList& params){
//     using val = const double;
// };

};