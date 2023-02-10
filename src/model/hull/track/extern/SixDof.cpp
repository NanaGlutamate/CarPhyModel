#include "SixDof.h"
#include "Turning.h"
#include "StraightAhead.h"
#include "TurningSixDof.h"
#include "conio.h"
#include <iostream>

namespace externModel::track{

void SixDof::setForce(double f1, double f0) {
    F.resize(3);
    this->F[0] = f0;
    this->F[1] = f1;
}

Sim_Vector SixDof::getTraction(DataStore param) {
    
    
    char m;
    double power = param.tracked_.power;
    if (_kbhit()) {
        m = _getch();
        if (m == 'w') {
            power += 0.2 * param.tracked_.maxpower;
            if (power >= param.tracked_.maxpower) {
                power = param.tracked_.maxpower;
                std::cout << std::endl << "----------" << "power �Ѵ����ֵ��" << "----------" << std::endl << std::endl;
            }
            else {
                std::cout << std::endl << "----------" << "power �� 20%" << "----------" << std::endl << std::endl;
            }
            
        }
        else if (m == 's') {
            power -= 0.2 * param.tracked_.maxpower;
            if (power <= 0) {
                power = 0;
                std::cout << std::endl << "----------" << "power �Ѵ���Сֵ��" << "----------" << std::endl << std::endl;
            }
            else {
                std::cout << std::endl << "----------" << "power �� 20%" << "----------" << std::endl << std::endl;
            }
            
        }
        else if (m == 'd') {
            F[0] -= 0.2 * ((double)F[0] + F[1]);
            F[1] += 0.2 * ((double)F[0] + F[1]);
            if (F[1] < -F[0]) {
                F[0] = -F[1];
                std::cout << std::endl << "----------" << "��ȫ����ת��" << "----------" << std::endl << std::endl;
            }
            else {
                std::cout << std::endl << "----------" << "��ת�������� �� " << "----------" << std::endl << std::endl;
            }
            
        }
        else if (m == 'a') {
            F[0] += 0.2 * ((double)F[0] + F[1]);
            F[1] -= 0.2 * ((double)F[0] + F[1]);
            if (F[0] < -F[1]) {
                F[1] = -F[0];
                std::cout << std::endl << "----------" << "��ȫ����ת��" << "----------" << std::endl << std::endl;
            }
            else {
                std::cout << std::endl << "----------" << "��ת�������� �� " << "----------" << std::endl << std::endl;
            }
           
        }

        while (1) {
            if (_kbhit()) {
                m = _getch();
                if (m == ' ') {
                    break;
                }
            }
        }
    }
    
    F[2] = power;
    return F;
}
std::tuple<SixDof::Vector3, SixDof::Vector3, SixDof::Vector3, SixDof::Vector3, double> SixDof::carMoveSimplify(DataStore param , const SixDof& sh) {
	/*Sim_Vector F = getTraction(param);*/
    
    /*double slope = ground.getslope(param.curpos_, param.cureuler_, param);*/
    int mode = 1;
    double dt = 0.001 ;
    /*if (mode == 0) {
        StraightAhead straight;
        std::tie(param.curpos_, param.cureuler_, param.curvel_, param.AngularVelocity) = straight.Tick(dt, F[0], F[1], param, sh, slope);
    }*/
    if (mode == 1 && fabs(param.cureuler_[0]) < 1e-6&&fabs(param.cureuler_[1]<1e-6)) {
        Turning turning;
        std::tuple<Vector3, Vector3, Vector3, Vector3> m;
        m = turning.Tick(param.dt, param.F[0], param.F[1], param, sh);
        std::tie(param.curpos_, param.cureuler_, param.curvel_, param.AngularVelocity) = m;
    }
    if (mode == 2 ||( mode == 1 && (fabs(param.cureuler_[0]) > 1e-6 || fabs(param.cureuler_[1] > 1e-6)))) {
        TurningSixDof turn;
        std::tie(param.curpos_, param.cureuler_, param.curvel_, param.AngularVelocity) = turn.Tick(param.dt, F[0], F[1], param, sh);
    }
    return std::tuple<Vector3, Vector3, Vector3, Vector3, double>(param.curpos_, param.cureuler_,param.curvel_, param.AngularVelocity, param.tracked_.power);
}


int SixDof::getMode(Sim_Vector F, double slope) {
    int mode = 0;
    if (fabs(F[0] - F[1]) < 1e-4) mode = 0;
    else if (fabs(slope - 0) < 1e-4) mode = 1;
    else if (fabs(slope - 0) > 1e-4) mode = 2;
    return mode;
}
std::tuple<SixDof::Vector3, SixDof::Vector3, SixDof::Vector3, SixDof::Vector3> SixDof::carMoveComplex(DataStore param) {
    Sim_Vector F = getTraction(param);
    double slope = ground.getslope(param.curpos_, param.cureuler_, param);
    int mode = getMode(F, slope);
    double dt = 0.05;
    std::tie(param.curpos_, param.cureuler_, param.curvel_, param.AngularVelocity) = Tick(dt, F[0], F[1], param);
    return std::tuple<Vector3, Vector3, Vector3, Vector3>(param.curpos_, param.cureuler_, param.curvel_, param.AngularVelocity);
}
std::tuple<SixDof::Vector3, SixDof::Vector3, SixDof::Vector3, SixDof::Vector3> SixDof::Tick(double dt, double Fo, double Fi, DataStore param) {
    return std::tuple<Vector3, Vector3, Vector3, Vector3>(param.curpos_, param.cureuler_, param.curvel_, param.AngularVelocity);
}

}