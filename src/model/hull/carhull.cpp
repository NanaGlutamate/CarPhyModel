#include <cmath>
#include <set>
#include <tuple>

#include "../tools/vector3.hpp"
#include "carhull.h"
#include "wheel/wheel.h"

namespace {

using namespace carphymodel::command;

constexpr size_t validMovingCommandMask =
    // COMMAND_TYPE::FORWARD,
    size_t(1) << static_cast<int>(COMMAND_TYPE::ACCELERATE) | size_t(1) << static_cast<int>(COMMAND_TYPE::DECELERATE) |
    size_t(1) << static_cast<int>(COMMAND_TYPE::BACKWARD) | size_t(1) << static_cast<int>(COMMAND_TYPE::STOP) |
    size_t(1) << static_cast<int>(COMMAND_TYPE::TURN) | size_t(1) << static_cast<int>(COMMAND_TYPE::ACCELERATE_TURN) |
    size_t(1) << static_cast<int>(COMMAND_TYPE::DECELERATE_TURN) |
    size_t(1) << static_cast<int>(COMMAND_TYPE::BACK_TURN) | size_t(1) << static_cast<int>(COMMAND_TYPE::FOLLOW_ROAD) |
    size_t(1) << static_cast<int>(COMMAND_TYPE::SET_ROAD);

}; // namespace

namespace carphymodel {

void HullSystem::tick(double dt, Components& c) {
    using namespace std;
    auto& damage = c.getSpecificSingleton<DamageModel>().value();
    if (damage.damageLevel == DAMAGE_LEVEL::K || damage.damageLevel == DAMAGE_LEVEL::KK) {
        return;
    }

    auto& optParam = c.getSpecificSingleton<WheelMotionParamList>();
    if (!optParam.has_value()) {
        return;
    }
    // TODO: track
    auto& param = optParam.value();

    // TODO: check
    Coordinate coordinate = c.getSpecificSingleton<Coordinate>().value();
    double direction = Quaternion::fromCompressedQuaternion(coordinate.attitude).getEuler().z;
    double speed = c.getSpecificSingleton<Hull>()->velocity.dot(coordinate.directionBodyToWorld(Vector3(1., 0., 0.)));
    for (auto&& [k, v] : c.getSpecificSingleton<CommandBuffer>().value()) {
        if ((validMovingCommandMask & size_t(1) << static_cast<int>(k)) == 0) {
            continue;
        }
        auto [param1, param2] = any_cast<tuple<double, double>>(v);
        // if (k == COMMAND_TYPE::ACCELERATE_TURN || k == COMMAND_TYPE::ACCELERATE) {
        //     param1 = max(param1, speed);
        // } else if (k == COMMAND_TYPE::DECELERATE_TURN || k == COMMAND_TYPE::DECELERATE) {
        //     param1 = min(param1, speed);
        // }
        auto& pathPlanningModelData = c.getSpecificSingleton<PathPlanningModel>().value();
        if (k == COMMAND_TYPE::FOLLOW_ROAD) {
            if (pathPlanningModelData.nextPoint == pathPlanningModelData.route.size()) {
                // finished
                continue;
            }
            auto maxSpeed = param1;
            auto routeDir =
                pathPlanningModelData.route[pathPlanningModelData.nextPoint] - pathPlanningModelData.prePoint;
            auto routeDis = routeDir.norm();
            auto routeDirNorm = routeDir / routeDis;

            auto finishedDis = (coordinate.position - pathPlanningModelData.prePoint).dot(routeDirNorm);
            if (finishedDis > routeDis) {
                if (pathPlanningModelData.nextPoint + 1 == pathPlanningModelData.route.size()) {
                    pathPlanningModelData.nextPoint++;
                    continue;
                }
                pathPlanningModelData.prePoint = pathPlanningModelData.route[pathPlanningModelData.nextPoint++];
                continue;
            }
            auto verticalDir = coordinate.position - pathPlanningModelData.prePoint -
                                ((coordinate.position - pathPlanningModelData.prePoint).dot(routeDirNorm)) * routeDirNorm;
            // auto VerticalRange = verticalDir.norm();
            auto expDir = routeDirNorm - verticalDir * 0.1;

            direction = atan2(expDir.y, expDir.x);
            // to make speed > 0
            speed = (coordinate.directionBodyToWorld({1, 0, 0}).dot(expDir.normalize()) + 1.1) / 2.1 * maxSpeed;
            if (routeDis - finishedDis < 80) {
                speed = fmax(3, (routeDis - finishedDis) / 80 * speed);
            }
            continue;
        } else if (k == COMMAND_TYPE::SET_ROAD) {
            pathPlanningModelData.route = EnvironmentInfoAgent{}.getRoute(coordinate.position, {param1, param2, 0});
            pathPlanningModelData.nextPoint = 1;
            auto firstSegmentDir = pathPlanningModelData.route[1] - pathPlanningModelData.route[0];
            pathPlanningModelData.prePoint = pathPlanningModelData.route[0] - firstSegmentDir;
            continue;
        } else {
            pathPlanningModelData.route.clear();
            pathPlanningModelData.nextPoint = 1;
        }
        if (k == COMMAND_TYPE::STOP) {
            speed = 0.;
        } else if (k == COMMAND_TYPE::TURN) {
            direction = param1;
        } else if (k == COMMAND_TYPE::ACCELERATE || k == COMMAND_TYPE::DECELERATE || k == COMMAND_TYPE::BACKWARD) {
            speed = param1;
        } else if (k == COMMAND_TYPE::ACCELERATE_TURN || k == COMMAND_TYPE::DECELERATE_TURN ||
                   k == COMMAND_TYPE::BACK_TURN) {
            speed = param1;
            direction = param2;
        }
        // TODO:
    }
    // size_t times = 1;
    // if (dt > 0.1) {
    //     times = size_t(std::ceil(dt / 0.1));
    // }
    // for (size_t i = 0; i < times; ++i) {
    WheelMoveSystem::tick(dt, c.getSpecificSingleton<Coordinate>().value(), c.getSpecificSingleton<Hull>().value(),
                          direction, speed, param);
};

} // namespace carphymodel