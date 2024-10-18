#include <iostream>
#include <tuple>

#include "../basetest.h"
#include "../../src/model/sensors/radar/myradar.h"

std::ostream& operator<<(std::ostream& o, const EntityInfo& e) {
    o << "{position: " << e.position << ", velocity: " << e.velocity << "}";
    return o;
}

int main() {
    using namespace std;
    carphymodel::CarModel model;
    buildBaseModel(model);
    get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[1]) =
        // EntityInfo{.position = {1000, 0, 0},
        EntityInfo{.position = {3000, 0, 0},
                   .velocity = {0, 0, 0},
                   .baseInfo = {BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N, 1, 1, /*0.6*/0.9, 3000.0}};
    auto& buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>().value();
    buffer.emplace(static_cast<carphymodel::command::COMMAND_TYPE>(2), any(tuple<double, double>(0, 1)));
    double detectrate = 0;
    double jd = 0;
    for (int i = 0; i < 1000; ++i) {
        model.tick(0.05);
        auto& tmp = model.components.getSpecificSingleton<carphymodel::ScannedMemory>().value()[1];
        if (get<0>(tmp) == 0) {
            detectrate += 1; // cout << "scanned list: " << get<1>(tmp) << endl;
            jd += sqrt((get<1>(tmp).position.x - 5000) * (get<1>(tmp).position.x - 5000) +
                   get<1>(tmp).position.y * get<1>(tmp).position.y);
        }
        else {
            ;//cout << "scanned list: {}" << endl;
        }
        get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[1]) =
            // EntityInfo{.position = {1000, 0, 0},
            EntityInfo{.position = {3000, 0, 0},
                       .velocity = {0, 0, 0},
                       .baseInfo = {BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N, 1, 1, 0.9, 3000.0}};
    }
    cout /*<< "探测距离："
                        << (get<1>(tmp).position -
                        model.components.getSpecificSingleton<Coordinate>().value().position).norm()*/
        << "目标定位精度："
            << jd/detectrate <<endl;
    cout << "detect rate: " << detectrate / 1000 << endl << "interference success rate: " << 1 - detectrate / 1000 << endl;
    return 0;
}