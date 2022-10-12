#include "basetest.h"

namespace test02{
//car builder test

int test_main(){
    using namespace std;
    CarModel m;
    CarBuilder builder;
    ifstream os("D:\\Desktop\\workspace\\12others\\2022-04-01CarModelling\\P0_carModel\\src\\config\\exampleCar.xml");
    string buffer;
    while(!(os.eof())){
        string tmp;
        getline(os, tmp);
        buffer.append(tmp);
    }
    //cout << buffer;
    builder.build(buffer, m);
    //断点查看结果
    return 0;
};
}