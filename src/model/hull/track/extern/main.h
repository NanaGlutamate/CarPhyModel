#pragma once

#include "SixDof.h"
#include <iostream>
#include <math.h>
#include <tuple>
#include "vectorn.hpp"

namespace externModel::track{

using E = double;
// using Vector2 = VectorN<E, 2>;

inline constexpr double min(double a, double b) {return (((a) < (b)) ? (a) : (b));}

namespace {
    // const double mass = 80000.0;
    // const double inertia = 100000.0;
    // const double PI = 3.14159265358979323;
    // const double g = 9.8;
    // const double muRoll = 0.05;
    // const double muSlide = 0.2;
    // const double muRollT = 0.2;
    double Fl = 40000, Fr = 40000;
    SixDof sixDOF;
    DataStore param;
    using Vector3 = Vector33;
}

double softSign(double x, double threshold = 1e-4) {
    if (x < -threshold) return -1;
    if (x > threshold) return 1;
    return x / threshold;
}
struct InputParams {
    double pos_x, pos_y;
    double vel_x, vel_y;
    double w;
};
extern "C" __declspec(dllexport) void __stdcall Init(InputParams inputparams) {//ƫ���� ����
    param.curvel_ = { inputparams.vel_x, inputparams.vel_y, 0};
    sixDOF.setPosition({ inputparams.pos_x, inputparams.pos_y, 0 }, param.pi / 2, param);
}

extern "C" __declspec(dllexport) void __stdcall SetInput(int flag) {
    if (flag == 1) {
        Fr = 200000;
        Fl = 00000;
    }
    else if (flag == 2) {
        Fr = 00000;
        Fl = 200000;
    }
    else if (flag == 0) {
        Fr = 100000;
        Fl = 100000;
    }
}

extern "C" __declspec(dllexport) void __stdcall Tick(double dt) {
    // std::tuple<Vector3, Vector3, Vector3, Vector3, double> tp({ 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.0);
    param.dt = dt;
    param.F[0] = Fl;
    param.F[1] = Fr;
    sixDOF.carMoveSimplify(param, sixDOF);
}
struct OUTPUT_PARAMS {
    double pos_x, pos_y;
    double vel_x, vel_y;
    double w;
};
extern "C" __declspec(dllexport) void __stdcall Getoutput(OUTPUT_PARAMS &output_params) {
    output_params.pos_x = param.curpos_[0];
    output_params.pos_y = param.curpos_[1];
    output_params.vel_x = param.curpos_[0];
    output_params.vel_y = param.curpos_[1];
    output_params.w = param.cureuler_[2];
}

}