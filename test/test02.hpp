#pragma once

#include "basetest.h"

namespace test02{
//car builder test

int test_main(){
    using namespace std;
    CarModel m;
    CarBuilder builder;
    builder.buildFromFile(__FILE__ + "/../../src/config/exampleCarV2.xml"s, m);
    //断点查看结果
    return 0;
};
}