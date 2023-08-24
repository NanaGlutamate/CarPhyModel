#pragma once

#include <ranges>

#include "basetest.h"
#include "src/model/hull/carhull.h"

namespace test13 {

using namespace carphymodel;

int test_main() {
    using namespace std;
    using namespace ranges;
    constexpr double dt = 0.1;
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
    ofstream f("data/route.txt");
    // auto& f = std::cout;
    f << "[";
    size_t tick = 0;
    Vector3 prePoint = {0, 0, 0};
    double direction, speed;
    for (auto p : route | views::transform(CoordTrans) | views::drop(1)) {
        constexpr auto maxSpeed = 20.;
        auto routeDir = p - prePoint;
        auto routeDis = routeDir.norm();
        auto routeDirNorm = routeDir / routeDis;
        while (++tick) {
            auto finishedDis = (coordinate.position - prePoint).dot(routeDirNorm);
            if (finishedDis > routeDis) {
                break;
            }
            auto verticalDir =
                coordinate.position - prePoint - ((coordinate.position - prePoint).dot(routeDirNorm)) * routeDirNorm;
            // auto VerticalRange = verticalDir.norm();
            auto expDir = routeDirNorm - verticalDir * 0.1;

            direction = atan2(expDir.y, expDir.x);
            // to make speed > 0
            speed = (coordinate.directionBodyToWorld({1, 0, 0}).dot(expDir.normalize()) + 1.1) / 2.1 * maxSpeed;
            if (routeDis - finishedDis < 80) {
                speed = fmax(3, (routeDis - finishedDis) / 80 * speed);
            }

            WheelMoveSystem::tick(dt, coordinate, hull, direction, speed, param);
            auto tmp = positionTrans(coordinate.position);
            f << "[" << std::to_string(tmp.longitude) << "," << std::to_string(tmp.latitude) << "],";
        }
        prePoint = p;
    }
    f << "]";
    return 0;
}

} // namespace test13