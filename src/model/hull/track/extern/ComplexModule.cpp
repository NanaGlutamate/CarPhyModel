#include "ComplexModule.h"

namespace externModel::track{

std::tuple<ComplexModule::Vector3, ComplexModule::Vector3, ComplexModule::Vector3, ComplexModule::Vector3> ComplexModule::Tick(double dt, double Fo, double Fi, DataStore param) {

	return std::tuple<ComplexModule::Vector3, ComplexModule::Vector3, ComplexModule::Vector3, ComplexModule::Vector3>();
}
void ComplexModule::addLocalForce(Vector33 F, Vector33 pos, DataStore param) {
    Coordinate co;
    Quaternion qu(param.cureuler_[2], param.cureuler_[0], param.cureuler_[1]);
    co.position = { 0.0,0.0,0.0 };
    co.altitude = { qu.x, qu.y, qu.z };
    Vector33 v = { param.curvel_[0], param.curvel_[1], param.curvel_[2] };
}
void ComplexModule::addTorque(Vector33 T) {

}

}