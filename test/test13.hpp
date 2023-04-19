#pragma once

#include <ranges>

#include "basetest.h"
#include "src/model/hull/carhull.h"

namespace test13 {

using namespace carphymodel;

struct RoutePoint {
    double longitude;
    double latitude;
};

inline std::vector<RoutePoint> route = {
    {113.550893, 22.216442}, {113.550947, 22.216902}, {113.550972, 22.216992}, {113.551016, 22.217051},
    {113.551062, 22.217094}, {113.551139, 22.217143}, {113.551251, 22.217197}, {113.55142, 22.21727},
    {113.551624, 22.217366}, {113.551809, 22.217462}, {113.551893, 22.217509}, {113.552011, 22.217604},
    {113.552056, 22.217664}, {113.552049, 22.21775},  {113.552042, 22.217856}, {113.55232, 22.21834},
    {113.552514, 22.218681}, {113.553002, 22.219538}, {113.553154, 22.219804}, {113.553187, 22.219866},
    {113.553207, 22.219924}, {113.553211, 22.219951}, {113.553214, 22.220002}, {113.553202, 22.220137},
    {113.553232, 22.220206}, {113.553265, 22.220264}, {113.553458, 22.220582}, {113.553509, 22.220698},
    {113.553512, 22.220807}, {113.554354, 22.222259}, {113.554275, 22.222295}, {113.553591, 22.221096},
    {113.553349, 22.220671}, {113.553295, 22.22061},  {113.553233, 22.220553}, {113.553166, 22.220493},
    {113.553082, 22.220434}, {113.553002, 22.22038},  {113.552869, 22.220368}, {113.55283, 22.220365},
    {113.552771, 22.220357}, {113.552707, 22.220348}, {113.552625, 22.220338}, {113.55164, 22.220194},
    {113.551412, 22.220172}, {113.551367, 22.220234}, {113.551371, 22.220268}, {113.551378, 22.220302},
    {113.551385, 22.220331}, {113.5514, 22.220369},   {113.551416, 22.220402}, {113.551509, 22.220582},
    {113.5516, 22.220762},   {113.551718, 22.220996}, {113.551808, 22.221254}, {113.551916, 22.221455},
    {113.551951, 22.221523}, {113.552012, 22.221635}, {113.55203, 22.221678},  {113.552259, 22.2222},
    {113.552495, 22.222733}, {113.552534, 22.222836}, {113.552574, 22.222932}, {113.552366, 22.222986},
    {113.55221, 22.223007},  {113.551664, 22.22308},  {113.550907, 22.223181}, {113.550515, 22.22314},
    {113.549852, 22.223069}, {113.549248, 22.223005}, {113.54921, 22.223001},  {113.549178, 22.222997},
    {113.549133, 22.222992}, {113.548519, 22.222927}, {113.548364, 22.22291},  {113.548203, 22.222886},
    {113.548058, 22.222867}, {113.547794, 22.222829}, {113.547758, 22.222824},
};

constexpr inline double rate = 111000.;

Vector3 CoordTrans(const RoutePoint &location) {
    return {
        rate * (location.latitude - route[0].latitude),
        rate * (location.longitude - route[0].longitude) * cos(carphymodel::DEG2RAD(route[0].latitude)),
        0,
    };
}

RoutePoint positionTrans(const Vector3 &position) {
    return {
        .longitude = position.y / (rate * cos(carphymodel::DEG2RAD(route[0].latitude))) + route[0].longitude,
        .latitude = position.x / rate + route[0].latitude,
    };
}

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