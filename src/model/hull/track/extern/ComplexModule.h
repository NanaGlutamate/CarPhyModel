#pragma once
#include "DataStore.h"
#include "vectorn.hpp"
#include "SixDof.h" 
#include <tuple>
#include "coordinate.h"

namespace externModel::track{

class ComplexModule
{
public:
	ComplexModule() :force({ 0.0,0.0,0.0 }), torque({ 0.0,0.0,0.0 }) {
	}
	using Vector3 = Vector33; // VectorN<double, 3>;
	void addLocalForce(Vector33 Force, Vector33 pos, DataStore param);
	void addTorque(Vector33 Force);
	std::tuple<Vector3, Vector3, Vector3, Vector3> Tick(double dt, double Fo, double Fi, DataStore param);
private:
	Vector3 force;
	Vector3 torque;
};

}