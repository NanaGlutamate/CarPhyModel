#include <string>

#include "../../tools/constant.hpp"
#include "../../tools/myassert.hpp"
#include "wheel.h"

namespace {

using namespace carphymodel;

// 投射到给定范围内
double clamp(double value, double min, double max) {
    if (value < min)
        value = min;
    if (value > max)
        value = max;
    return value;
}

double clamp(double value, double bound) {
    my_assert(bound >= 0., "Bad Clamp Bound: " + std::to_string(bound));
    // if (bound < 0)
    //     bound = -bound;
    if (value < -bound)
        value = -bound;
    if (value > bound)
        value = bound;
    return value;
}

double softClamp(double value, double bound) { return atan(value * (PI / 2.) / bound) / (PI / 2.) * bound; }

double softClamp(double value, double min, double max) {
    double mid = (min + max) / 2;
    return softClamp(value - mid, max - mid) + mid;
}

// 获取当前转角时角速度与速度比例
double angularLinearRatio(double rad, double LENGTH) { return tan(rad) / LENGTH; }

// 获取符号
int sign(double value) {
    if (value > INF_SMALL)
        return 1;
    if (value < -INF_SMALL)
        return -1;
    return 0;
}

double angleDiff(double angleExpect, double angleNow) {
    double tmp = angleExpect - angleNow;
    if (tmp > PI)
        tmp -= 2 * PI;
    if (tmp < -PI)
        tmp += 2 * PI;
    return tmp;
}

bool equal(double a, double b) { return fabs(a - b) < INF_SMALL; }

}; // namespace

namespace carphymodel {

// 各种限制导致的速度减小不会小于该速度，避免停车
constexpr inline double MIN_SPEED = 3.;

// 采用前右下坐标系，速度和转角分别控制以支持原地转方向盘、缓慢转弯、行进中变向等操作
// 由于动力学之上会采用控制器，无需考虑显式欧拉法的不稳定问题，出于简单考虑直接使用显式欧拉法

// 影响因素：
// 前轮转向：1. 最大转角 2. 最大转动速度 3. 最大侧向加速度 4. 到达目标方向时能回正
// 速度：1. 最大直线速度 2. 最大减速加速度 3. 最大侧向加速度 4. 最大前进加速度 5.
// 坡度带来的重力加速度分量

// 由于前轮转向需要时间，故速度也需要受最大侧向加速度约束
// 控制方法：符合上述约束的条件下尽可能快速地向目标调整
// TODO：倒车
void WheelMoveSystem::tick(double dt, Coordinate &baseCoordinate, Hull &hull, double expectYaw, double expectSpeed,
                           WheelMotionParamList &params) {
    // 参数计算
    double speed = baseCoordinate.directionWorldToBody(hull.velocity).x;
    double yaw_now = Quaternion::fromCompressedQuaternion(baseCoordinate.attitude).getEuler().z;
    // 目标偏航角与当前偏航角的差，目标偏右为正
    double exp_yaw_diff = angleDiff(expectYaw, yaw_now); // atan2(local_exp_direction.y, local_exp_direction.x);
    // 倒车
    if (speed < 0)
        exp_yaw_diff = -exp_yaw_diff;
    Vector3 front_direction = baseCoordinate.directionBodyToWorld(Vector3(1., 0., 0.));
    // 当前坡度
    double slope = env.getSlope(baseCoordinate.position, front_direction);
    // 重力向前加速度分量
    double gravity_acceleration = -G * slope / sqrt(1 + slope * slope);

    // 更新转角(方向盘)

    double angle_restriction = params.MAX_ANGLE;
    // 转弯加速度约束
    double acceleration_restriction_on_angle =
        (fabs(speed) < INF_SMALL) ? PI / 2 : atan(params.MAX_LATERAL_ACCELERATION * params.LENGTH / (speed * speed));
    angle_restriction = fmin(angle_restriction, acceleration_restriction_on_angle);
    // 剩余回正时间约束
    double angular_speed = hull.palstance.norm() * sign(hull.palstance.z);
    double time_last = /* 2 * */ exp_yaw_diff / angular_speed;
    if (time_last < 0.)
        time_last = INFINITY;
    double rotate_restriction_on_angle = time_last * params.ROTATE_SPEED;
    angle_restriction = fmin(angle_restriction, rotate_restriction_on_angle);
    // 约束下目标转角
    double exp_angle = clamp(params.angle + exp_yaw_diff, angle_restriction);
    // double exp_angle = clamp(exp_yaw_diff > 0 ? params.MAX_ANGLE : -params.MAX_ANGLE,
    //                          fmin(aviliable_angle, time_last * params.ROTATE_SPEED));
    // double exp_angle =
    //     clamp(params.angle + exp_yaw_diff, multiMin(params.MAX_ANGLE, aviliable_angle,
    //     time_last * params.ROTATE_SPEED));
    // 转动速度约束下可行转动量
    double delta_radius = clamp(exp_angle - params.angle, params.ROTATE_SPEED * dt);
    params.angle += delta_radius;

    // 更新速度(油门)

    // 转弯速度太快，减速
    double restriction_rate = 1.;
    if (equal(params.angle, rotate_restriction_on_angle) || equal(params.angle, acceleration_restriction_on_angle))
        restriction_rate = 0.95;
    double speed_restriction = params.MAX_LINEAR_SPEED;
    // 最大侧向加速度约束下的最大速度
    double acceleration_restriction_on_speed =
        (fabs(params.angle) < INF_SMALL)
            ? params.MAX_LINEAR_SPEED
            : restriction_rate * sqrt(params.MAX_LATERAL_ACCELERATION * params.LENGTH / fabs(tan(params.angle)));
    speed_restriction = fmin(speed_restriction, acceleration_restriction_on_speed);
    // TODO: 回正速度(time_last * params.ROTATE_SPEED >= params.MAX_ANGLE && aviliable_angle
    // >= params.MAX_ANGLE)
    double rotate_restriction_on_speed =
        /* 2 *  */ restriction_rate * (params.ROTATE_SPEED * exp_yaw_diff) /
        (fabs(params.angle) * angularLinearRatio(params.angle, params.LENGTH));
    // 若不需要回正,无限制
    if (params.angle * delta_radius <= 0)
        rotate_restriction_on_speed = INFINITY;
    // // 当前转向与期望转向相反,无限制(若直接停车会使得小幅度摆动时速度太小)
    // if (rotate_restriction_on_speed <= 0.)
    //     rotate_restriction_on_speed = INFINITY;
    // 当前转向与期望转向相反,或限制速度太小(由于速度减小时方向盘角度可以变大,进一步又限制速度大小,若无限制减小速度最终会导致停车)
    if (rotate_restriction_on_speed <= MIN_SPEED)
        rotate_restriction_on_speed = MIN_SPEED;
    speed_restriction = fmin(speed_restriction, rotate_restriction_on_speed);
    // 约束下的期望速度
    expectSpeed = clamp(expectSpeed, speed_restriction);
    // 速度变化量
    double delta_speed;
    if (speed >= 0.) {
        delta_speed = clamp(expectSpeed - speed, (gravity_acceleration - params.MAX_BRAKE_ACCELERATION) * dt,
                            (gravity_acceleration + params.MAX_FRONT_ACCELERATION) * dt);
    } else {
        delta_speed = clamp(expectSpeed - speed, (gravity_acceleration - params.MAX_FRONT_ACCELERATION / 2) * dt,
                            (gravity_acceleration + params.MAX_BRAKE_ACCELERATION) * dt);
    }
    // small fix: 当前角度与期望角度差距过大且速度不小时不加速
    if (delta_speed > 0. && speed >= MIN_SPEED && fabs(exp_yaw_diff) > PI / 10) {
        delta_speed = 0.;
    }
    // 计算新速度和角速度
    double new_speed = speed + delta_speed;

    // 更新状态

    // 角速度只与速度和前轮转角有关
    double new_angular_speed = new_speed * angularLinearRatio(params.angle, params.LENGTH);
    // 计算新朝向
    double dyaw = angular_speed * dt;
    const Vector3 new_front_direction = baseCoordinate.directionBodyToWorld(Vector3(cos(dyaw), sin(dyaw), 0.));
    // 计算新位置
    const Vector3 new_position_unlanded = baseCoordinate.position + hull.velocity * dt;
    // 投影到地面
    const Vector3 new_position = {new_position_unlanded.x, new_position_unlanded.y,
                                  -env.getAltitude(new_position_unlanded)};
    // 计算新随体坐标系姿态四元数
    double new_yaw = atan2(new_front_direction.y, new_front_direction.x);
    double new_pitch = atan(env.getSlope(new_position, new_front_direction));
    double new_roll = atan(env.getSlope(new_position, new_front_direction.out({0., 0., -1.})));
    const Quaternion new_altitude(new_roll, new_pitch, new_yaw);
    // 更新坐标系
    baseCoordinate.position = new_position;
    baseCoordinate.attitude = new_altitude.toCompressedQuaternion();
    // 更新速度和角速度
    hull.velocity = baseCoordinate.directionBodyToWorld(Vector3(new_speed, 0., 0.));
    hull.palstance = baseCoordinate.directionBodyToWorld(Vector3(0., 0., new_angular_speed));

    return;
}

} // namespace carphymodel