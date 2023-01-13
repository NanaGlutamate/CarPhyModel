#include "Turning.h"
namespace {
    double softSign(double x, double threshold = 1e-6) {
        if (x < -threshold) return -1;
        if (x > threshold) return 1;
        return x / threshold;
    };
}

namespace externModel::track{

std::tuple<Turning::Vector3, Turning::Vector3, Turning::Vector3, Turning::Vector3> Turning::Tick(double dt, double Fi, double Fo, DataStore param, const SixDof & threeDof) {
    auto localv = threeDof.get3VelocityLocal(param);
    auto globalv = threeDof.getVelocityGlobal(param);
    auto globalw = threeDof.getAngularVelocityGlobal(param);
    Vector3 position = param.curpos_;
    Vector3 euler = param.cureuler_;
    double v = localv.norm();
    float muRoll = param.muroll;
    float muSlide = param.muslide;
    double Fmax = param.tracked_.power / v;
    if (Fo + Fi - Fmax > 0) {
        Fo = Fo * Fmax / (Fo + Fi);
        Fi = Fi * Fmax / (Fo + Fi);
    }
    if (fabs(globalw[2]) < 1e-4) globalw[2] = 1e-4;
    double Iz = param.tracked_.mass * (param.tracked_.width * param.tracked_.width + param.tracked_.BodyLength * param.tracked_.BodyLength)/12;
    if (fabs(Iz) < 1e-4) Iz = 1e-4;
    addForceLocal({Fo, 0, 0 }, {0, param.tracked_.width/2,0},param);//����Ĵ�
    addForceLocal({ Fi, 0, 0 }, { 0, -param.tracked_.width / 2,0 }, param);//�Ҳ��Ĵ� 
    addForceLocal({ -softSign(localv[0]) * muRoll * param.g * param.tracked_.mass, -softSign(localv[1]) * muSlide * param.g * param.tracked_.mass, 0 }, { 0, 0, 0 }, param);//Ħ������
    addTorque(-softSign(threeDof.getOmega(param)) * muSlide * param.tracked_.BodyLength *param.g * param.tracked_.mass / 3 * fabs(threeDof.getOmega(param)));//Ħ��ת��
    

    Vector3 vhalf = globalv + (force / param.tracked_.mass) * dt / 2;
    E omegahalf = param.AngularVelocity[2] + (torque / Iz) * dt / 2;
    position += vhalf * dt;
    euler[2] += omegahalf * dt;
    globalv = vhalf + (force / param.tracked_.mass) * dt / 2;
    globalw[2] = omegahalf + (torque / Iz) * dt / 2;
    torque = 0;
    force = {0.0,0.0,0.0};
    return std::tuple<Vector3, Vector3, Vector3, Vector3>(position, euler, globalv, globalw);
}
void  Turning::addForceLocal(const Turning::Vector3& force, const  Turning::Vector3& position, DataStore param) {
    this->force += Vector3{ force[0] * cos(param.cureuler_[2]) - force[1] * sin(param.cureuler_[2]),
                           force[0] * sin(param.cureuler_[2]) + force[1] * cos(param.cureuler_[2]),0 };
    this->torque += (position[0] * force[1] - position[1] * force[0]);
};
void Turning::addTorque(E torque) {
    this->torque += torque;
};

}