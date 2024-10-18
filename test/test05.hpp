#include "basetest.h"

namespace test05{
// carhull test
using namespace std;

const double G = 9.8;
const double PI = 3.1415926535897932384626433;
// 前后轴距
const double LENGTH = 5.0;
// 最大转角约束
const double MAX_ANGLE = 30.0 * PI / 180.0;
// 车轮转动速度约束
const double ROTATE_SPEED = MAX_ANGLE / 1.5;
// 最大直线速度约束
const double MAX_LINEAR_SPEED = 30.0;
// 最大前向加速度约束
const double MAX_FRONT_ACCELERATION = MAX_LINEAR_SPEED / 10.0;
// 最大减速加速度约束
const double MAX_BRAKE_ACCELERATION = MAX_LINEAR_SPEED / 12.0;
// 最大转弯向心加速度(侧向加速度)约束
const double MAX_LATERAL_ACCELERATION = G * 0.1;
// 油量
const double OIL_REMAIN = 1000.0;
// 耗油量
const double OIL_CONSUMPTION = 250000.0;

WheelMotionParamList car1{
    0.,
    LENGTH,
    MAX_ANGLE,
    ROTATE_SPEED,
    MAX_LINEAR_SPEED,
    MAX_FRONT_ACCELERATION,
    MAX_BRAKE_ACCELERATION,
    MAX_LATERAL_ACCELERATION,
    OIL_REMAIN,
    OIL_CONSUMPTION,
};

int test_main(){
    ofstream ofs("D:/codeproject/CarPhyModel/data/data05_1.txt");
    ofs << "[";
    //CarHullSystem car;
    WheelMoveSystem car;
    Coordinate baseCoordinate{Vector3(0,0,0), Vector3(0,0,0)};
    Hull hull{Vector3(0,0,0), Vector3(0,0,0)/*, 1, Vector3(1,1,1)*/};
    
    /*Vector3 exp_direction1{1,1,0};*/
    double exp_direction1{1};
    double exp_speed1{1};

    int s1 = 1000;

    /*Vector3 exp_direction2{1,0,0};*/
    double exp_direction2{0};
    double exp_speed2{5};

    int s2 = 2000;

    //Vector3 exp_direction3{1,-1,0};
    double exp_direction3{-1};
    double exp_speed3{10};

    int s3 = 2500;

    for(int i=0;i<s1;i++){
        car.tick(0.1, baseCoordinate, hull, exp_direction1, exp_speed1, car1);
        // cout <<  i << ", " << baseCoordinate.position << "," << hull.velocity << ","
        //                    << Quaternion(baseCoordinate.altitude, true).getEuler() << "," << hull.palstance.norm() << ","  << car.getRadius() << endl;
        ofs << /*baseCoordinate.position*/car1.OIL_REMAIN << ",";
    }
    for(int i=s1;i<s2;i++){
        car.tick(0.1, baseCoordinate, hull, exp_direction2, exp_speed2, car1);
        // cout <<  i << ", " << baseCoordinate.position << "," << hull.velocity << ","
        //                    << Quaternion(baseCoordinate.altitude, true).getEuler() << "," << hull.palstance.norm() << ","  << car.getRadius() << endl;
        ofs << /*baseCoordinate.position*/ car1.OIL_REMAIN << ",";
    }
    for(int i=s2;i<s3;i++){
        car.tick(0.1, baseCoordinate, hull, exp_direction3, exp_speed3, car1);
        // cout <<  i << ", " << baseCoordinate.position << "," << hull.velocity << ","
        //                    << Quaternion(baseCoordinate.altitude, true).getEuler() << "," << hull.palstance.norm() << ","  << car.getRadius() << endl;
        ofs << /*baseCoordinate.position*/ car1.OIL_REMAIN << ",";
    }
    ofs << "]";

    return 0;
};

};