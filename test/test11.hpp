#pragma once

#include "basetest.h"
#include "../src/model/geoinfo.hpp"

namespace test11{

using namespace carphymodel::geoInfo;

constexpr Vector3 tmp = CoordinateOrder<N, U, E>{1, 2, 3};

int test_main(){
    std::cout << tmp << std::endl;
    return 0;
}

}