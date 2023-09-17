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
        EntityInfo{.position = {1000, 0, 0},
                   .velocity = {0, 0, 0},
                   .baseInfo = {BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}};
    auto& buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>().value();
    buffer.emplace(static_cast<carphymodel::command::COMMAND_TYPE>(10), any(tuple<double, double>(0, 1)));

    for (int i = 0; i < 10; ++i) {
        model.tick(0.05);
        auto& tmp = model.components.getSpecificSingleton<carphymodel::ScannedMemory>().value()[1];
        if(get<0>(tmp) == 0)cout << "scanned list: " << get<1>(tmp) << endl;
        else cout << "scanned list: {}" << endl;
    }
    return 0;
}