#pragma once
#include "DataStore.h"
#include "vectorn.hpp"
#include "SixDof.h" 

namespace externModel::track{

class TurningSixDof
{
public:
	using Vector3 = Vector33;// VectorN<double, 3>;
	std::tuple<Vector3, Vector3, Vector3, Vector3> Tick(double dt, double Fi, double Fo, DataStore param, const SixDof& sh);
    void add6ForceLocal(const Vector3& force, const Vector3& position, DataStore param) {
        Coordinate co;
        Quaternion qu(param.cureuler_[2], -param.cureuler_[0], param.cureuler_[1]);
        co.position = { 0.0,0.0,0.0 };
        co.altitude = { qu.x, qu.y, qu.z };
        Vector33 Flocal = { force[0], force[1], force[2] };
        Vector33 Fglobal = co.positionBodyToWorld(Flocal);
        this->force += co.positionBodyToWorld(Flocal);
        this->torque += {0,0, (position[0] * force[1] - position[1] * force[0])};
    };
    void add3Torque(Vector33 torque) {
        this->torque += torque;
    };
	Vector33 torque = { 0.0,0.0,0.0 };
	Vector33 force = { 0.0,0.0,0.0 };
};

}