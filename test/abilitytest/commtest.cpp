#include <iostream>
#include <tuple>
#include <string_view>

#include "../../src/model/sensors/radar/myradar.h"
#include "../../src/model/communication/communicationfactory.hpp"
#include "../basetest.h"

std::ostream& operator<<(std::ostream& o, const EntityInfo& e) {
    o << "{position: " << e.position << ", velocity: " << e.velocity << "}";
    return o;
}

int main() {
    using namespace std;
    Vector3 self{0, 0, 0};
    auto target = EntityInfo{.position = {1000, 0, 0},
                             .velocity = {0, 0, 0},
                             .baseInfo = {BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}};

    auto comm = CommunicationFactory::getProduct("antenna");
    for (int i = 0; i < 100; ++i) {
        if (comm->sendMessage(self, target.position))
            cout << "comm list: " << target << endl;
        else
            cout << "comm list: {}" << endl;
    }
    return 0;
}