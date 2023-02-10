#pragma once
#ifndef SIXDOF_
#define SIXDOF_

#include "DataStore.h"
#include "vectorn.hpp"
#include <tuple>
#include "coordinate.h"
#include "GroundData.hpp"

namespace externModel::track{

class SixDof
{
public:
    //! ��ȡ����������Ϊp��ĳ��ĶԵ��ٶ�����������ϵ�е�����
    //! @param p �õ�����
    //! @return �ٶ�����
    using E = double;
    using Vector3 = Vector33;// VectorN<E, 3>;
    SixDof() :force({0.0,0.0,0.0}), torque(0) {
    }
    Vector3 getAngularVelocityGlobal(DataStore param)const {
        return param.AngularVelocity;
    }
    Vector3 getVelocityGlobal(DataStore param)const
    {
        return param.curvel_;
    }
    Vector3 get3VelocityLocal(DataStore param, Vector3 p = { 0.0, 0.0, 0.0 }) const {
        return Vector3({ param.curvel_[0] * cos(param.cureuler_[2]) + param.curvel_[1] * sin(param.cureuler_[2]),
            -param.curvel_[0] * sin(param.cureuler_[2]) + param.curvel_[1] * cos(param.cureuler_[2]), 0 }) //ǣ���ٶ�
            + Vector3({ -p[1], p[0], 0 }) * param.AngularVelocity[2]; //ת���ٶ�
    };
    Vector33 get6VelocityLoval(DataStore param, Vector3 euler) const{
        Coordinate co;
        Quaternion qu(euler[2], -euler[0], euler[1]);
        co.position = { 0.0,0.0,0.0 };
        co.altitude = { qu.x, qu.y, qu.z };
        Vector33 v = { param.curvel_[0], param.curvel_[1], param.curvel_[2] };
        return co.positionWorldToBody(v);
    }
    //! get rotate speed
    //! @return omega
    E getOmega(DataStore param) const {
        return param.AngularVelocity[2];
    };
    //! set position and phi
    //! @param position position
    //! @param phi angle
    std::tuple<Vector3, Vector3> setPosition(Vector3 position, E phi, DataStore param) {
        param.curpos_ = position;
        param.cureuler_[2] = phi;
        return std::tuple<Vector3, Vector3>(param.curpos_, param.cureuler_);
    };
    //! add force(gloabal coordinate) to force buffer
    //! @param force force(global coordinate)
    void addForceGlobal(const Vector3& force) {
        this->force += force;
    };
    //! add force(local coordinate) to force buffer
    //! @param force force(local coordinate)
    //! @param position position(local coordinate) the force applied
    void addForceLocal(const Vector3& force, const Vector3& position, DataStore param) {
        this->force += Vector3{ force[0] * cos(param.cureuler_[2]) - force[1] * sin(param.cureuler_[2]),
                               force[0] * sin(param.cureuler_[2]) + force[1] * cos(param.cureuler_[2]),0 };
        this->torque += (position[0] * force[1] - position[1] * force[0]);
    };
    void add6ForceLocal(const Vector3& force, const Vector3& position, DataStore param) {
        Coordinate co;
        Quaternion qu(param.cureuler_[2],-param.cureuler_[0], param.cureuler_[1]);
        co.position = { 0.0,0.0,0.0 };
        co.altitude = { qu.x, qu.y, qu.z };
        Vector33 Flocal = {force[0], force[1], force[2] };
        Vector33 Fglobal = co.positionBodyToWorld(Flocal);
        this->force3 += co.positionBodyToWorld(Flocal);
        this->torque3 += {force[0] * position[1], -force[0]*position[0], (position[0] * force[1] - position[1] * force[0])};
    };
    //! add turque to Toquer buffer
    //! @param torque torque applied
    void addTorque(E torque) {
        this->torque += torque;
    };
    double softSign(double x, double threshold = 1e-6)const{
        if (x < -threshold) return -1;
        if (x > threshold) return 1;
        return x / threshold;
    };
    std::tuple<Vector3, Vector3,Vector3,Vector3, double> carMoveSimplify(DataStore param, const SixDof& sh);
    std::tuple<SixDof::Vector3, SixDof::Vector3, SixDof::Vector3, SixDof::Vector3> carMoveComplex(DataStore param);
    std::tuple<SixDof::Vector3, SixDof::Vector3, SixDof::Vector3, SixDof::Vector3> Tick(double dt, double Fo, double Fi, DataStore param);
    void setForce(double f1, double f0);
    double torque = 0;
    Vector3 force = {0.0,0.0,0.0};
    GroundData ground;
    Vector33 torque3 = { 0.0,0.0,0.0 };
    Vector33 force3 = { 0.0,0.0,0.0 };
    Sim_Vector F;
private:
    Sim_Vector getTraction(DataStore param);
    int getMode(Sim_Vector F, double slope);
};

}

#endif // !SIXDOF_