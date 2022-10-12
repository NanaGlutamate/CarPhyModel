#include "basetest.h"

namespace test07{

    int test_main() {
        using namespace std;
        CarModel m1, m2;
        CarBuilder builder;
        ifstream os("D:\\Desktop\\workspace\\12others\\2022-04-01CarModelling\\P0_carModel\\src\\config\\exampleCar.xml");
        ofstream ofs("D:\\Desktop\\workspace\\12others\\2022-04-01CarModelling\\P0_carModel\\data\\data06.txt");
        ofs << "[";
        string buffer;
        while (!(os.eof())) {
            string tmp;
            getline(os, tmp);
            buffer.append(tmp);
        }
        builder.build(buffer, m1);
        builder.build(buffer, m2);

        cout << "model build" << endl;

        m1.baseCoordinate.position = { 0., 0., 0. };
        m1.expDirection = { 1., 0., 0. };
        m1.expSpeed = 20.;

        m2.baseCoordinate.position = { 200000., 20., 0. };
        m2.expDirection = { -1., 0., 0. };
        m2.expSpeed = 20.;

        EntityInfo init1{ m1.baseCoordinate.position, m1.hull.velocity, ENTITY_TYPE::CAR, 2, 0. };
        EntityInfo init2{ m2.baseCoordinate.position, m2.hull.velocity, ENTITY_TYPE::CAR, 2, 0. };
        m1.scannedMemory.insert({ 2, init2 }); //((VID)2, {m2.baseCoordinate.position, m2.hull.velocity, 0, 2, 0.});
        m2.scannedMemory.insert({ 1, init2 });

        for (int i = 0; i < 20000; ++i) {
            m1.scannedMemory[2].position = m2.baseCoordinate.position;
            m1.scannedMemory[2].velocity = m2.hull.velocity;
            m2.scannedMemory[1].position = m1.baseCoordinate.position;
            m2.scannedMemory[1].velocity = m1.hull.velocity;

            m1.tick(0.5);
            m2.tick(0.5);

            if (i % 100 == 0) {
                ofs << "(" << m1.baseCoordinate.position << "," << m2.baseCoordinate.position << ","
                           << m1.scannedMemory[2].lastScanned << "," << m2.scannedMemory[1].lastScanned << ","
                           << (m1.baseCoordinate.position - m2.baseCoordinate.position).norm() << "),";
            }
        }
        ofs << "]";
        return 0;
    }

}