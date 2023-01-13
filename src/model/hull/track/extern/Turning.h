#pragma once
#ifndef TURNING_
#define TURNING_

#include "DataStore.h"
#include "vectorn.hpp"
#include "SixDof.h"
#include <tuple>

namespace externModel::track{

class Turning
{
public:
    using E = double;
    using Vector3 = Vector33;// VectorN<E, 3>;
    std::tuple<Vector3, Vector3, Vector3, Vector3> Tick(double dt, double Fi, double Fo, DataStore param, const SixDof & threeDof);
    Vector3 force = {0.0,0.0,0.0};
    double torque = 0;
    void  addForceLocal(const Turning::Vector3& force, const  Turning::Vector3& position, DataStore param);
    void  addTorque(E torque);
};

}

#endif // !TURNING_