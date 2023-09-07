#include <iostream>
#include <tuple>

#include "../basetest.h"

int main() {
    using namespace std;
    carphymodel::CarModel model;
    buildBaseModel(model);
    get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[1]) = EntityInfo{
        .position = {1000, 0, 0},
        .velocity = {0, 0, 0}, .baseInfo = {BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}
    };
    auto &buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>().value();
    buffer.emplace(static_cast<carphymodel::command::COMMAND_TYPE>(10), any(tuple<double, double>(0, 1)));
    model.tick(0.1);
    auto& buffer2 = model.components.getSpecificSingleton<carphymodel::EventBuffer>().value();
    if (auto it = buffer2.find(string("FireDataOut")); it != buffer2.end()) {
        FireEvent tmp = any_cast<FireEvent>(it->second);
        cout << format("carphymodel send fireEvent: {{weapon: {}, from: {}({}, {}, {}), to: ({}, {}, {})}}",
                       tmp.weaponName, 0, tmp.position.x, tmp.position.y, tmp.position.z, tmp.target.x,
                       tmp.target.y, tmp.target.z)
             << endl;
        Vector3 src = tmp.position;
        tmp.position = tmp.target;
        tmp.target = src;
        model.components.getSpecificSingleton<carphymodel::FireEventQueue>()->push_back(tmp);
    }
    model.tick(0.1);
    int level = static_cast<int>(model.components.getSpecificSingleton<carphymodel::DamageModel>()->damageLevel);
    cout << format("damage level from 0 to {}", level) << endl;
    return 0;
}