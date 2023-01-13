#pragma once
#ifndef STRAIGHTAHEAD_
#define STRAIGHTAHEAD_

#include "vectorn.hpp"
#include <tuple>
#include "DataStore.h"
#include "SixDof.h"

namespace externModel::track{

class StraightAhead
{
public:
    using E = double;
    using Vector3 = Vector33;// VectorN<E, 3>;
    std::tuple<Vector3, Vector3, Vector3, Vector3> Tick(float dt, double Fi, double Fo, DataStore param, const SixDof& sh, double slope);
    void addForceLocal(const StraightAhead::Vector3& force, const StraightAhead::Vector3& position, DataStore param);
    double torque = 0;
    Vector3 force = { 0.0,0.0,0.0 };
};

}

#endif // !STRAIGHTAHEAD_