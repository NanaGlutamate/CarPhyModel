#include "basetest.h"

namespace test07{

    int test_main() {
        using namespace std;
        CarModel m1;
        CarBuilder builder;
        builder.buildFromFile("D:\\Desktop\\FinalProj\\Code\\CarPhyModel\\src\\config\\car.xml", m1, true);

        cout << "model build" << endl;

        for (int i = 0; i < 20000; ++i) {
            m1.tick(0.5);
        }
        return 0;
    }

}