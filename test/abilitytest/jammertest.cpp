#include <iostream>
#include <tuple>

#include "../../src/model/sensors/radar/myradar.h"
#include "../basetest.h"

std::ostream& operator<<(std::ostream& o, const EntityInfo& e) {
    o << "{position: " << e.position << ", velocity: " << e.velocity << "}";
    return o;
}

int main() {
    using namespace std;
    carphymodel::CarModel model;
    buildBaseModel(model);

    Coordinate self{};
    EntityInfo e{.position = {1000, 0, 0},
                .velocity = {10, 0, 0},
                .baseInfo = {BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}};
    SensorData sensor{};
    Hull hull{};
    double jammerPower = 10;  // 0;

    MyRadar radar;

    while (1) {
        if (!radar.isDetectable(self, e, sensor, hull, jammerPower)) {
            cout << "range: " << e.position << endl;
            break;
        } else {
            e.position += e.velocity;
        }
    }
    return 0;
}