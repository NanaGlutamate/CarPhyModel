#pragma once
#include "DataStore.h"
#include "vectorn.hpp"
#include <memory>

#include "../../../environment.h"

namespace externModel::track{

class GroundData
{
public:
	GroundData():env(){};
	GroundData(GroundData&& d) = delete;
	GroundData(const GroundData& d) = delete;
	~GroundData() = default;
	using E = double;
	using Vector3 = Vector33;// VectorN<double, 3>;
	double getslope(Vector3 curpos, Vector3 dir, DataStore param)const{
		return env.getSlope(curpos, dir);
	};
	double getHeight(Vector3 curpos, DataStore param)const{
		return env.getAltitude(curpos);
	}
	/*Array_env env;*/
	double getGlobalHeight(double posx, double posy)const{
		return env.getAltitude(Vector3{posx, posy, 0});
	};
	//void WriteRaw() ;
	//void ChaZhi(int N, int xlen, int ylen);//��ֵ��������ǰ��ά����
	//double* aray;
private:
	carPhyModel::EnvironmentInfoAgent env;
	//double *grounddata;
};

}