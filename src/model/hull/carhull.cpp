#include <cmath>
#include <set>
#include <tuple>

#include "../tools/vector3.hpp"
#include "carhull.h"
#include "wheel/wheel.h"

namespace {

using namespace carphymodel::command;

const std::set<COMMAND_TYPE> validMovingCommand{
    // COMMAND_TYPE::FORWARD,
    COMMAND_TYPE::ACCELERATE,
    COMMAND_TYPE::DECELERATE,
    COMMAND_TYPE::BACKWARD,
    COMMAND_TYPE::STOP,
    COMMAND_TYPE::TURN,
    COMMAND_TYPE::ACCELERATE_TURN,
    COMMAND_TYPE::DECELERATE_TURN,
    COMMAND_TYPE::BACK_TURN,
};

}; // namespace

namespace carphymodel {

void HullSystem::tick(double dt, Components &c) {
    using namespace std;
    auto &optParam = c.getSpecificSingleton<WheelMotionParamList>();
    if (!optParam.has_value())
        return;
    // TODO: track
    auto &param = optParam.value();

    // TODO: check
    auto coordinate = c.getSpecificSingleton<Coordinate>().value();
    double direction = Quaternion::fromCompressedQuaternion(coordinate.attitude).getEuler().z;
    double speed = c.getSpecificSingleton<Hull>()->velocity.norm();
    for (auto &&[k, v] : c.getSpecificSingleton<InputBuffer>().value()) {
        // if (!validMovingCommand.contains(k)) {
        //     continue;
        // }
        auto [param1, param2] = any_cast<tuple<double, double>>(v);
        if (k == COMMAND_TYPE::ACCELERATE_TURN || k == COMMAND_TYPE::ACCELERATE){
            param1 = max(param1, speed);
        }else if (k == COMMAND_TYPE::DECELERATE_TURN || k == COMMAND_TYPE::DECELERATE){
            param1 = min(param1, speed);
        }
        if (k == COMMAND_TYPE::STOP) {
            speed = 0.;
        } else if (k == COMMAND_TYPE::TURN) {
            direction = param1;
        } else if (k == COMMAND_TYPE::ACCELERATE || k == COMMAND_TYPE::DECELERATE ||
                   k == COMMAND_TYPE::BACKWARD) {
            speed = param1;
        } else if (k == COMMAND_TYPE::ACCELERATE_TURN || k == COMMAND_TYPE::DECELERATE_TURN ||
                   k == COMMAND_TYPE::BACK_TURN) {
            speed = param1;
            direction = param2;
        }
        // TODO:
    }
    WheelMoveSystem::tick(dt, c.getSpecificSingleton<Coordinate>().value(),
                          c.getSpecificSingleton<Hull>().value(), direction, speed, param);
};

} // namespace carphymodel