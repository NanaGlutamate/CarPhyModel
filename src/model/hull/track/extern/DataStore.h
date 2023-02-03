#pragma once
#ifndef DATASTORE_
#define DATASTORE_

#include <vector>
#include <iostream>
#include "vectorn.hpp"

namespace externModel::track{

typedef std::vector<float> Sim_Vector;
class DataStore
{
public:
	using E = double;
	using Vector3 = Vector33;
	Vector3 cureuler_;
	Vector3 curpos_;
	Vector3 curdir_;
	Vector3 curvel_;
	Vector3 AngularVelocity;//0俯仰，1滚转，2偏航
	float muroll, muslide;
	float curspeed_;
	double g;
	double pi = 3.1415926;
	struct TrackedVars {
		double power;//总功率
		double length;//车总长
		double BodyLength;//车体长
		double mass;//质量
		double width;//车宽
		double height;//车高
		double MaxSpeed;//最大车速
		double MaxDistance;//最大车程
		double max_vertical_wall;//最大垂直墙高度
		double max_trench;//最大壕沟长度
		double max_vertical_angle;//最大坡度
		double maxpower;
	};
	TrackedVars tracked_;
	double dt=0.0;
	double F[2] = {0.0, 0.0};
	void init();
};

}

#endif // !DATASTORE_