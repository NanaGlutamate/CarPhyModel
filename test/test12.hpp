#pragma once

#include "basetest.h"
#include "src/model/hull/carhull.h"

namespace test12 {

using namespace carphymodel;

int test_main() {
    using namespace std;
    constexpr double dt = 0.1;
    double direction, speed;
    for (double K1 = 0; K1 < 10; K1 += 1.)
        for (double K2 = 0; K2 < 10; K2 += 1.) {
            Coordinate coordinate{
                {0, 0, 0},
                {0, 0, 0},
            };
            Hull hull{
                {0, 0, 0},
                {0, 0, 0},
            };
            WheelMotionParamList param{
                0, 5, 0.523, 0.349, 40, 3, 2.5, 1,
            };
            struct Target {
                Vector3 position;
                Vector3 velocity;
                void tick(double dt) { position += dt * velocity; }
            } target_ {
                {-10, 0, 0},
                {-5, 0, 0},
                // {10, 0, 0},
                // {0, 5, 0},
            };
            struct RingTarget {
                Vector3 center;
                double speed;

                Vector3 position;
                Vector3 velocity;
                void tick(double dt) { 
                    double r = (position - center).norm();
                    Quaternion q({0, 0, 1}, speed / r * dt);
                    position = q.rotate(Quaternion::fromVector(position - center)).toVector() + center;
                    velocity = Vector3({0, 0, speed / r}).out(position - center);
                }
            } target {
                {0, 0, 0},
                -3,
                {20, 0, 0},
                {0, -3, 0},
            };
            ofstream f(format("data/{}-{}.txt", K1, K2));
            ofstream fv(format("data/{}-{}v.txt", K1, K2));
            f << "[";
            fv << "[";
            size_t tick = 0;
            while (++tick) {
                // K1 = 1; K2 = 5;
                bool target_in_front = (coordinate.directionBodyToWorld({1, 0, 0}).dot((target.position - coordinate.position).normalize())) >= -0.1;
                bool same_direction = (coordinate.directionBodyToWorld({1, 0, 0}).dot(target.velocity.normalize())) >= -0.1;
                auto v_exp = target.velocity + K1 * (target.position - coordinate.position) +
                            K2 * (target.velocity - hull.velocity);
                if(target_in_front && same_direction) {
                    // speed = v_exp.norm() * (v_exp.normalize().dot(coordinate.directionBodyToWorld({1, 0, 0})) + 1) / 2;
                    // if(speed < 5.)speed = 5.;
                    speed = v_exp.norm();
                    direction = atan2(v_exp.y, v_exp.x);
                }else if(!target_in_front && same_direction) {
                    // speed = v_exp.norm() / 2;
                    // direction = atan2(v_exp.y, v_exp.x);
                    speed = target.velocity.norm() / 2;
                    direction = atan2(target.velocity.y, target.velocity.x);
                }else if(target_in_front && !same_direction) {
                    speed = -5;
                    direction = atan2(target.velocity.y, target.velocity.x);
                }else{
                    speed = -5;
                    direction = atan2(target.velocity.y, target.velocity.x);
                }
                if (tick == 2000) {
                    break;
                }
                if(tick == 450){
                    ;
                }
                if (coordinate.position.x > 10.) {
                    ;
                }
                target.tick(dt);
                WheelMoveSystem::tick(dt, coordinate, hull, direction, speed, param);
                // std::cout << hull.velocity.norm() << ",";
                f << "(" << coordinate.position.x << "," << coordinate.position.y << "),";
                fv << hull.velocity.norm() << ",";
                // std::cout << "[" << coordinate.position << "," <<
                // Quaternion::fromCompressedQuaternion(coordinate.attitude) << "]," <<
                // std::endl;
            }
            f << "]";
            fv << "]";
        }
    return 0;
}

} // namespace test12