#include <iostream>
#include <tuple>

#include "../../src/model/sensors/radar/myradar.h"
#include "../basetest.h"
#include "src/model/environment.h"

std::ostream& operator<<(std::ostream& o, const EntityInfo& e) {
    o << "{position: " << e.position << ", velocity: " << e.velocity << "}";
    return o;
}

int main() {
    using namespace std;
    using carphymodel::Vector3;
    carphymodel::CarModel model;
    buildBaseModel(model);

    Coordinate self{};
    EntityInfo e{.position = {1000, 0, 0},
                 .velocity = {0, 0, 0},
                 .baseInfo = {BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}};
    SensorData sensor{};
    Hull hull{};
    double jammerPower = 10; // 0;

    MyRadar radar;
    std::vector<Vector3> target = {
        {1000, 500, 0}, 
        {10000, 800, 0}, 
        {5000, 3000, 0}, 
        {3000, 4000, 0}, 
        {6000, 7000, 0}, 
        {8000, 1000, 0}, 
        {2000, 1000, -1}, 
        {4000, 2000, 0}, 
        {3000, 9000, 0},
    };

    for (auto&& tar : target) {
        e.position = tar;
        bool env = carphymodel::EnvironmentInfoAgent{}.getIntervisibility(tar, Vector3{0, 0, 0});
        cout << std::format("({}, {}, {}, {}),", tar.x, tar.y, int(env && radar.isDetectable(self, e, sensor, hull, 0)),
                            int(env && radar.isDetectable(self, e, sensor, hull, jammerPower)))
             << endl;
    }
    return 0;
}