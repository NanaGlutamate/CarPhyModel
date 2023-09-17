#include <iostream>
#include <tuple>

#include "../basetest.h"

int main() {
    using namespace std;
    carphymodel::CarModel model;
    //carphymodel::CarBuilder::buildFromFile("D:\\Desktop\\FinalProj\\Code\\CarPhyModel\\src\\config\\car_protection.xml",
    //                                       model);
    carphymodel::CarBuilder::buildFromFile("D:\\Desktop\\FinalProj\\Code\\CarPhyModel\\src\\config\\car.xml",
                                           model);
    FireEvent tmp;
    tmp.weaponName = "125mmHE";
    tmp.position = {1000, 0, 0};
    tmp.target = {0, 0, 0};
    tmp.velocity = {-100, 0, 0};
    tmp.range = 1000;
    model.components.getSpecificSingleton<carphymodel::FireEventQueue>()->push_back(tmp);
    model.tick(0.1);
    int level = static_cast<int>(model.components.getSpecificSingleton<carphymodel::DamageModel>()->damageLevel);
    cout << format("damage level from 0 to {}", level) << endl;
    return 0;
}