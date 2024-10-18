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
#include <unordered_map>
#include <optional>

using namespace carphymodel;

inline void printCSValueMap(const std::unordered_map<std::string, std::any>& v) {
    using namespace std;
    cout << "{";
    bool start = false;
    for (auto& [k, v] : v) {
        if (!start) {
            start = true;
        } else {
            cout << ", ";
        }
        cout << k << ": ";
        if (v.type() == typeid(bool)) {
            cout << (bool)std::any_cast<bool>(v);
        } else if (v.type() == typeid(int8_t)) {
            cout << (int64_t)std::any_cast<int8_t>(v);
        } else if (v.type() == typeid(uint8_t)) {
            cout << (uint64_t)std::any_cast<uint8_t>(v);
        } else if (v.type() == typeid(int16_t)) {
            cout << (int64_t)std::any_cast<int16_t>(v);
        } else if (v.type() == typeid(uint16_t)) {
            cout << (uint64_t)std::any_cast<uint16_t>(v);
        } else if (v.type() == typeid(int32_t)) {
            cout << std::any_cast<int32_t>(v);
        } else if (v.type() == typeid(uint32_t)) {
            cout << std::any_cast<uint32_t>(v);
        } else if (v.type() == typeid(int64_t)) {
            cout << std::any_cast<int64_t>(v);
        } else if (v.type() == typeid(uint64_t)) {
            cout << std::any_cast<uint64_t>(v);
        } else if (v.type() == typeid(float)) {
            cout << std::any_cast<float>(v);
        } else if (v.type() == typeid(double)) {
            cout << std::any_cast<double>(v);
        } else if (v.type() == typeid(std::string)) {
            cout << std::any_cast<std::string>(v);
        } else if (v.type() == typeid(std::unordered_map<std::string, std::any>)) {
            printCSValueMap(std::any_cast<std::unordered_map<std::string, std::any>>(v));
        } else if (v.type() == typeid(std::vector<std::any>)) {
            auto tmp = std::any_cast<std::vector<std::any>>(v);
            cout << "[";
            bool start_ = false;
            size_t cnt = 0;
            for (auto&& item : tmp) {
                if (!start_) {
                    start_ = true;
                } else {
                    cout << ", ";
                }
                printCSValueMap({{std::to_string(cnt), item}});
                cnt++;
            }
            cout << "]";
        } else {
            cout << "unknown";
        }
    }
    cout << "}";
}

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

inline void buildBaseModel(const std::string& XMLFilePath, carphymodel::CarModel& model) {
    carphymodel::CarBuilder::buildFromFile(XMLFilePath, model);
    //"D:\\codeproject\\CarPhyModel\\src\\config\\car_protection.xml"
}