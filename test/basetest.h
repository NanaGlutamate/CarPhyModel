#pragma once

#include <assert.h>
#include <iostream>
#include <string>
#include <array>
#include <fstream>
#include <iterator>
#include <tuple>
#include "../src/model/carbuilder.h"
#include "../src/extern/rapidxml-1.13/rapidxml.hpp"
#include "../src/model/tools/vector3.hpp"
#include "../src/model/tools/coordinate.hpp"
#include "../src/model/hull/wheel/wheel.h"
#include "../src/model/ammunition/ammunitionfactory.hpp"
#include "../src/extern/pfr.hpp"
#include "../src/model/framework/component.hpp"

using namespace carPhyModel;

std::ostream& operator<<(std::ostream& os, const Vector3& v){
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Quaternion& v){
    os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return os;
}