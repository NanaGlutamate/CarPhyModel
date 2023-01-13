#include "StraightAhead.h"
#include "GroundData.hpp"
namespace {
    int flag = 0, flag1= 0;
    double pitch1 = 0;
    
}

namespace externModel::track{

std::tuple<StraightAhead::Vector3, StraightAhead::Vector3, StraightAhead::Vector3, StraightAhead::Vector3>StraightAhead::Tick(float dt, double Fi, double Fo, DataStore param, const SixDof& sh, double slope) {
    Vector3 position = param.curpos_;
    Vector3 euler = param.cureuler_;
    Vector3 globalv = param.curvel_;
    Vector3 globalw = param.AngularVelocity;
    
    //SixDof sh;//ֱ��
    Vector3 localv = sh.get3VelocityLocal(param);
    
    double v = globalv.norm();
    double Fmax = param.tracked_.power / v;
    if (Fo + Fi - Fmax > 0) {
        Fo = Fo * Fmax / (Fo + Fi);
        Fi = Fi * Fmax / (Fo + Fi);
    }
    float muroll = param.muroll;
    float muslide = param.muslide;
    Vector3 direction = {0.0,0.0,0.0};
    double curslope = slope;

    double lheightf = sh.ground.getHeight({param.tracked_.BodyLength/2 , param.tracked_.width / 2, 0 }, param);
    double rheightf = sh.ground.getHeight({ param.tracked_.BodyLength/2, -param.tracked_.width / 2, 0 }, param);
    double lheighta = sh.ground.getHeight({ -param.tracked_.BodyLength / 2 , param.tracked_.width / 2, 0 }, param);
    double rheighta = sh.ground.getHeight({ -param.tracked_.BodyLength / 2, -param.tracked_.width / 2, 0 }, param);
    double lheight = (lheightf + lheighta) / 2;
    double rheight = (rheightf + rheighta) / 2;
    if (Fo == 0 && Fi == 0) {
        muroll = muslide;
    }
    addForceLocal({ Fo + Fi - sh.softSign(localv[0]) * muroll * param.tracked_.mass * param.g * cos(curslope),0,0 }, { 0.0,0.0,0.0 }, param);
    addForceLocal({ -param.tracked_.mass * param.g * sin(curslope),0,0 }, { 0.0,0.0,0.0 }, param);
    
    Vector3 vhalf = globalv + force / param.tracked_.mass * dt / 2;
    position += vhalf * dt;
    globalv = vhalf + force / param.tracked_.mass * dt / 2;
    if (fabs(euler[0] - curslope) > 1e-6 && flag == 0 || fabs(euler[0] - curslope) > 0.1) {
        double m = 0, t = 0;
        m = curslope - euler[0];
        t = param.tracked_.BodyLength / v;
        globalw[0] = (double)m / t;
        euler[0] += globalw[0] * dt;
        flag = (int)param.tracked_.BodyLength / (v * t);
    }
    else if (flag) {
        if (euler[0] <= curslope && curslope >= 0)
            euler[0] = euler[0] + globalw[0] * dt;
        if (euler[0] > curslope && curslope >= 0) {
            euler[0] = curslope; flag = 0;
        }
        if (euler[0] > curslope && curslope < 0) {
            euler[0] = curslope; flag = 0;
        }
        if (euler[0] <= curslope && curslope < 0) {
            euler[0] = euler[0] + globalw[0] * dt;
        }
        flag--;
    }
    globalv[2] = globalv.norm() * sin(euler[0]);
    position[2] += vhalf.norm() * dt * sin(euler[0]);
    //euler[0] = -euler[0];//����ϵ����任
    curslope = euler[1];
    if (fabs(lheight - rheight) > 1e-6) {
        curslope = asin((lheight - rheight)/param.tracked_.width);
    }
    if (fabs(euler[1] - curslope) > 1e-6 && flag1 == 0 || fabs(euler[1] - curslope) > 0.1) {
        double m = 0, t = 0;
        m = curslope - euler[1];
        t = param.tracked_.BodyLength / v;
        globalw[1] = (double)m / t;
        euler[1] += globalw[1] * dt;
        flag1 = (int)param.tracked_.BodyLength / (v * t);
    }
    else if (flag1) {
        if (euler[1] <= curslope && curslope >= 0)
            euler[1] = euler[1] + globalw[1] * dt;
        if (euler[1] > curslope && curslope >= 0) {
            euler[1] = curslope; flag1 = 0;
        }
        if (euler[1] > curslope && curslope < 0) {
            euler[1] = curslope; flag1 = 0;
        }
        if (euler[1] <= curslope && curslope < 0) {
            euler[1] = euler[1] + globalw[1] * dt;
        }
        flag1--;
    }
    /*if (fabs(position[2] - sh.ground.getGlobalHeight(position[0], position[1])) > 0.1) {
        position[2] = sh.ground.getGlobalHeight(position[0], position[1]);
    }*/
    return std::tuple<Vector3, Vector3, Vector3, Vector3>(position, euler, globalv, globalw);
}
void StraightAhead::addForceLocal(const StraightAhead::Vector3& force, const StraightAhead::Vector3& position, DataStore param) {
    this->force += StraightAhead::Vector3{ force[0] * cos(param.cureuler_[2]) - force[1] * sin(param.cureuler_[2]),
                           force[0] * sin(param.cureuler_[2]) + force[1] * cos(param.cureuler_[2]),0 };
    this->torque += (position[0] * force[1] - position[1] * force[0]);
};

}