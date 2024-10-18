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

inline std::vector<RoutePoint> route1 = {
    {113.54413, 22.217469}, {113.543943, 22.217417}, {113.54372, 22.217342}, {113.543626, 22.21732},
    {113.54347, 22.217302}, {113.543232, 22.217294}, {113.542764, 22.217295}, {113.542723, 22.217311},
    {113.542698, 22.217332}, {113.54268, 22.217359}, {113.542674, 22.217384}, {113.542669, 22.217455},
    {113.542669, 22.217472}, {113.542693, 22.218385}, {113.542707, 22.218715}, {113.542695, 22.218886},
    {113.54268, 22.219006}, {113.542681, 22.219112}, {113.542714, 22.21927}, {113.543145, 22.220718},
    {113.543318, 22.221331}, {113.543337, 22.221399}, {113.543593, 22.222306}, {113.543695, 22.222623},
    {113.543908, 22.223247}, {113.54435, 22.224534}, {113.544545, 22.225102}, {113.544613, 22.225475},
    {113.544475, 22.225456}, {113.544181, 22.225415}, {113.543965, 22.225388}, {113.543826, 22.225372},
    {113.542814, 22.225229}, {113.542526, 22.225188}
};

inline std::vector<RoutePoint> route2 = {
    {113.549142, 22.223753}, {113.549179, 22.22347},  {113.549202, 22.223289}, {113.549209, 22.223239},
    {113.549234, 22.22309},  {113.549248, 22.223005}, {113.54921, 22.223001},  {113.549178, 22.222997},
    {113.549133, 22.222992}, {113.548519, 22.222927}, {113.548364, 22.22291},  {113.548342, 22.223066},
    {113.548183, 22.224189}, {113.548175, 22.224259}, {113.548015, 22.22541},  {113.547953, 22.225854},
    {113.547804, 22.225833}, {113.547651, 22.225815}, {113.547404, 22.225786}, {113.54675, 22.225709},
    {113.546672, 22.2257},   {113.54556, 22.225573},  {113.545487, 22.225565}, {113.544728, 22.225487},
    {113.544613, 22.225475}, {113.544475, 22.225456}, {113.544181, 22.225415}, {113.543965, 22.225388},
    {113.543826, 22.225372}, {113.542814, 22.225229}, {113.542526, 22.225188}};

inline std::vector<RoutePoint> route3 = {
    {113.54413, 22.217469},  {113.543943, 22.217417}, {113.54372, 22.217342},  {113.543626, 22.21732},
    {113.54347, 22.217302},  {113.543232, 22.217294}, {113.542764, 22.217295}, {113.542723, 22.217311},
    {113.542698, 22.217332}, {113.54268, 22.217359},  {113.542674, 22.217384}, {113.542669, 22.217455},
    {113.542669, 22.217472}, {113.542693, 22.218385}, {113.542707, 22.218715}, {113.542695, 22.218886},
    {113.54268, 22.219006},  {113.542681, 22.219112}, {113.542714, 22.21927},  {113.543145, 22.220718},
    {113.543318, 22.221331}, {113.543337, 22.221399}, {113.543593, 22.222306}, {113.543934, 22.22235},
    {113.544122, 22.222375}, {113.544633, 22.22243},  {113.544812, 22.222449}, {113.545188, 22.222491},
    {113.546433, 22.222649}, {113.546511, 22.222659}, {113.547034, 22.22273},  {113.547517, 22.222793},
    {113.547794, 22.222829}, {113.547972, 22.222855}, {113.548058, 22.222867}, {113.548203, 22.222886},
    {113.548364, 22.22291},  {113.548342, 22.223066}, {113.548183, 22.224189}, {113.548258, 22.224197},
    {113.548315, 22.224204}, {113.548404, 22.224214}, {113.548835, 22.224262}, {113.548892, 22.224268},
    {113.548912, 22.22427},  {113.54907, 22.224307},  {113.549142, 22.223753}, {113.549142, 22.223753}};

inline std::vector<RoutePoint> route = {
    {113.54413, 22.217469},  {113.543943, 22.217417}, {113.54372, 22.217342},  {113.543626, 22.21732},
    {113.54347, 22.217302},  {113.543232, 22.217294}, {113.542764, 22.217295}, {113.542723, 22.217311},
    {113.542698, 22.217332}, {113.54268, 22.217359},  {113.542674, 22.217384}, {113.542669, 22.217455},
    {113.542669, 22.217472}, {113.542693, 22.218385}, {113.542707, 22.218715}, {113.542695, 22.218886},
    {113.54268, 22.219006},  {113.542563, 22.21899},  {113.542484, 22.218977}, {113.542327, 22.21895},
    {113.542274, 22.218943}, {113.541848, 22.218878}, {113.54161, 22.218807},  {113.539858, 22.218282},
    {113.539494, 22.218128}, {113.537972, 22.217488}, {113.537579, 22.217337}, {113.537334, 22.217243},
    {113.537282, 22.217222}, {113.537179, 22.217181}, {113.536353, 22.21684},  {113.535626, 22.216536},
    {113.53384, 22.215941},  {113.532518, 22.21562},  {113.530323, 22.215179}, {113.526918, 22.214512},
    {113.525649, 22.214263}, {113.525012, 22.214142}, {113.523953, 22.213921}, {113.523353, 22.213825},
    {113.523084, 22.213747}, {113.522868, 22.213671}, {113.522464, 22.213529}, {113.522027, 22.213321},
    {113.521654, 22.213054}, {113.521224, 22.212723}, {113.520973, 22.212483}, {113.520738, 22.212227},
    {113.520445, 22.211868}, {113.520179, 22.211527}, {113.519522, 22.210759}, {113.519436, 22.21068},
    {113.519317, 22.210589}, {113.519211, 22.210527}, {113.519035, 22.210443}};

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
    std::vector<Vector3> routedata;
    ofstream f1("D:/codeproject/CarPhyModel/test/data/routedata.txt");
    // auto& f1 = std::cout;
    f1 << "{";
    for (auto p : route | views::transform(CoordTrans)) {
        routedata.push_back(p);
        f1 << "{" << std::to_string(p.x) << "," << std::to_string(p.y) << "," << std::to_string(p.z) << "},";
    }
    f1 << "}";
    f1.close();
    Coordinate coordinate{
        {0, 0, 0},
        {0, 0, 0},
    };
    Hull hull{
        {0, 0, 0},
        {0, 0, 0},
    };
    WheelMotionParamList param{
        0, 5, 0.523, 0.349, 40, 3, 2.5, 1, 1000.0, 250.0,
    };
    ofstream f("D:/codeproject/CarPhyModel/test/data/route.txt");
    // auto& f = std::cout;
    f << "[";
    size_t tick = 0;
    Vector3 prePoint = {0, 0, 0};
    double direction, speed;
    int i = 1;
    for (auto p : route | views::transform(CoordTrans) | views::drop(1)) {
        constexpr auto maxSpeed = 20.;
        auto routeDir = p - prePoint;
        auto routeDis = routeDir.norm();
        auto routeDirNorm = routeDir / routeDis;
        //下一段路径方向
        if (i + 1 >= route.size())
        {
            break;
        }
        auto routeDirnext = CoordTrans(route[i + 1]) - p;
        auto routeDisnext = routeDirnext.norm();
        auto routeDirnextNorm = routeDirnext / routeDisnext;

        while (++tick) {
            auto finishedDis = (coordinate.position - prePoint).dot(routeDirNorm);
            if (finishedDis > routeDis) {
                break;
            }
            auto verticalDir =
                coordinate.position - prePoint - ((coordinate.position - prePoint).dot(routeDirNorm)) * routeDirNorm;
            // auto VerticalRange = verticalDir.norm();
            auto expDir = routeDirNorm - verticalDir * 0.1;
            //当距离小于xxm且下一段路径方向与当前路径方向相反时，则切换到下一段路径，这个3.85是经验值，是试验出来的，可以根据实际情况调整
            auto vel = hull.velocity.dot(routeDirNorm);
            if ((routeDis - finishedDis < 15/*vel * 3.85*/) && routeDirnext.dot(routeDir) < 0) {
                break; 
            }
            direction = atan2(expDir.y, expDir.x);
            // to make speed > 0
            speed = (coordinate.directionBodyToWorld({1, 0, 0}).dot(expDir.normalize()) + 1.1) / 2.1 * maxSpeed;
            //当距离小于80m且下一段路径方向与当前路径方向夹角大于30度，控制减速
            //TODO:减速的过程应该与夹角有关，而不是简单根据距离
            if (routeDis - finishedDis < 80 /*&& routeDirnextNorm.dot(routeDirNorm) < 0.866*/) {
                speed = fmax(3, (routeDis - finishedDis) / 80 * speed);
            }

            WheelMoveSystem::tick(dt, coordinate, hull, direction, speed, param);
            auto tmp = positionTrans(coordinate.position);
            f << "[" << std::to_string(tmp.longitude) << "," << std::to_string(tmp.latitude) << "],";
        }
        prePoint = p;
        i++;
    }
    f << "]";
    //继续在txt文件中输出routedata的内容；
    
    auto routetest = EnvironmentInfoAgent{}.getRoute({0, 0, 0}, CoordTrans(route[route.size() - 1]));
    //控制台打印出routetest
    for (auto p : routetest)
    {
        std::cout << p.x << " " << p.y << std::endl;
    }
    return 0;
}

} // namespace test13