#pragma once

#include "tools/vector3.hpp"

namespace carPhyModel {

class Environment {
private:
    friend class EnvironmentInfoAgent;
    static Environment* getInstance() {
        static Environment e;
        return &e;
    };
    Environment() {};
    Environment(const Environment&) = delete;
    Environment& operator=(const Environment&) = delete;
    double getAltitude(const Vector3& pos)const { return 0.; };
    double getSlope(const Vector3& pos, const Vector3& dir)const { return 0.; };
    bool getIntervisibility(const Vector3& pos1, const Vector3& pos2)const { return pos1.z >= 0. && pos2.z >= 0.; };
};

class EnvironmentInfoAgent {
private:
    Environment* env;
public:
    EnvironmentInfoAgent() :env(Environment::getInstance()) {};

    void changeEnvironmentSupplier(Environment* n) { env = n; return; };

    //! 采用北东地坐标系
    //! @param pos: 位置坐标(北东地)
    //! @return 位置在地表的投影相对某固定平面的高度(上为正)
    double getAltitude(const Vector3& pos) { return env->getAltitude(pos); };

    //! 采用北东地坐标系
    //! @param pos: 位置坐标(北东地)
    //! @param dir: 方向向量(北东地)
    //! @return 斜率(坡比，上坡为正)
    double getSlope(const Vector3& pos, const Vector3& dir) { return env->getSlope(pos, dir); };

    //! 采用北东地坐标系
    //! @param pos1: 位置坐标1(北东地)
    //! @param pos2: 位置坐标2(北东地)
    //! @return 是否可见
    bool getIntervisibility(const Vector3& pos1, const Vector3& pos2) { return env->getIntervisibility(pos1, pos2); };
};

}