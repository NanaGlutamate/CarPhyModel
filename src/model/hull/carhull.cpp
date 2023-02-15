#include <cmath>

#include "carhull.h"
#include "wheel/wheel.h"

namespace {};

namespace carphymodel {

void HullSystem::tick(double dt, Components &c) {
    auto &optParam = c.getSpecificSingleton<WheelMotionParamList>();
    if (!optParam.has_value())
        return;
    auto &param = optParam.value();

    WheelMoveSystem::tick(dt, c.getSpecificSingleton<Coordinate>().value(),
                          c.getSpecificSingleton<Hull>().value(), {}, 0, param);
};

} // namespace carphymodel