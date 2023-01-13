#include "TurningSixDof.h"
#include <tuple>
#include "GroundData.hpp"
namespace {
    int flag = 0;
    int flag1 = 0;
}

namespace externModel::track{

std::tuple<TurningSixDof::Vector3, TurningSixDof::Vector3, TurningSixDof::Vector3, TurningSixDof::Vector3> TurningSixDof::Tick(double dt, double Fi, double Fo, DataStore param, const SixDof& sh) {
	Vector3 position = param.curpos_;
	Vector3 euler = param.cureuler_;
	Vector3 globalw = param.AngularVelocity;
	Vector3 globalv = param.curvel_;
	//SixDof sh;
	double v = globalv.norm();
	double muRoll = param.muroll;
	double muSlide = param.muslide;
	double Fmax = param.tracked_.power / v;
	if (Fo + Fi - Fmax > 0) {
		Fo = Fo * Fmax / (Fo + Fi);
		Fi = Fi * Fmax / (Fo + Fi);
	}
	
	Vector33 localv = sh.get6VelocityLoval(param, euler);
	double curslope = sh.ground.getslope(position, euler, param);
	/*double Fx = Fo + Fi - sh.softSign(localv.x)*param.tracked_.mass * param.g * muRoll - param.g * sin(curslope);
	double Fy = param.tracked_.mass * param.g * muSlide * cos(curslope);*/

	add6ForceLocal({ Fo, 0, 0 }, { 0, param.tracked_.width / 2,0 }, param);//����Ĵ�
	add6ForceLocal({ Fi, 0, 0 }, { 0, -param.tracked_.width / 2 ,0}, param);//�Ҳ��Ĵ� 
	add6ForceLocal({ -sh.softSign(localv[0]) * muRoll * param.g * param.tracked_.mass * cos(euler[0])*cos(euler[1]), -sh.softSign(localv[1]) * muSlide * param.g * param.tracked_.mass* cos(euler[0])*cos(euler[1]),0 }, { 0, 0, 0 }, param);//Ħ������
	add6ForceLocal({0, 0, param.g * param.tracked_.mass * cos(euler[0]) * cos(euler[1]) }, { 0,0,0 }, param);
	this->force.z += param.tracked_.mass * param.g;
	add3Torque({ 0, 0, -sh.softSign(sh.getOmega(param)) * muSlide * param.tracked_.BodyLength * param.g * param.tracked_.mass / 3 * cos(euler[0]) * cos(euler[1])* fabs(sh.getOmega(param)) });//Ħ��ת��
	double Iz = param.tracked_.mass * (param.tracked_.width * param.tracked_.width + param.tracked_.BodyLength * param.tracked_.BodyLength) / 12;
	double x = this->force.x;
	Vector3 force; force[0] = this->force.x; force[1] = this->force.y; force[2] = this->force.z;
	Vector3 torque; torque[0] = this->torque.x; torque[1] = this->torque.y; torque[2] = this->torque.z;
	Vector3 vhalf = globalv + (force / param.tracked_.mass) * dt / 2;
	double omegahalf = param.AngularVelocity[2] + (torque[2] / Iz) * dt / 2;
	position += vhalf * dt;
	euler[2] += omegahalf * dt;
	//euler[0] = curslope * sin(euler[2]);
	//euler[1] = curslope * cos(euler[2]);
	globalv = vhalf + (force / param.tracked_.mass) * dt / 2;
	globalw[2] = omegahalf + (torque[2] / Iz) * dt / 2;
	/*if (fabs(position[2] - sh.ground.getGlobalHeight(position[0], position[1])) > 1e-4) {
		position[2] = sh.ground.getGlobalHeight(position[0], position[1]);
	}*/
    double lheightf = sh.ground.getHeight({ param.tracked_.BodyLength / 2 , param.tracked_.width / 2, 0 }, param);
    double rheightf = sh.ground.getHeight({ param.tracked_.BodyLength / 2, -param.tracked_.width / 2, 0 }, param);
    double lheighta = sh.ground.getHeight({ -param.tracked_.BodyLength / 2 , param.tracked_.width / 2, 0 }, param);
    double rheighta = sh.ground.getHeight({ -param.tracked_.BodyLength / 2, -param.tracked_.width / 2, 0 }, param);
    double lheight = (lheightf + lheighta) / 2;
    double rheight = (rheightf + rheighta) / 2;
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
    if (fabs(lheight - rheight) > 1e-4) {
        curslope = atan2(lheight - rheight, param.tracked_.width);
    }
    if (abs(euler[1] - curslope) > 1e-6 && flag1 == 0 || fabs(euler[1] - curslope) > 0.1) {
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
	return std::tuple<Vector3, Vector3, Vector3, Vector3>(position, euler, globalv, globalw);
}

}