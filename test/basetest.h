#pragma once

#include "../src/extern/pfr.hpp"
#include "../src/extern/rapidxml-1.13/rapidxml.hpp"
#include "../src/model/ammunition/ammunitionfactory.hpp"
#include "../src/model/carbuilder.h"
#include "../src/model/framework/component.hpp"
#include "../src/model/hull/wheel/wheel.h"
#include "../src/model/tools/coordinate.hpp"
#include "../src/model/tools/vector3.hpp"
#include "src/model/tools/constant.hpp"
#include <algorithm>
#include <array>
#include <assert.h>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <tuple>

using namespace carphymodel;

std::ostream &operator<<(std::ostream &os, const Vector3 &v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Quaternion &v) {
    // os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    auto e = v.getEuler();
    os << std::format("(yaw: {}, pitch: {}, roll: {})", e.z, e.y, e.x);
    return os;
}