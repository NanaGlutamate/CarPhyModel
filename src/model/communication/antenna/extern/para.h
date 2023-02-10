#pragma once
#include <string>
#include <sstream>  

namespace externModel::comm{

using namespace std;

template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}
struct Fixed_para {
	double Comm_Set_A;
	int Comm_Set_i;
	double Comm_Set_azi;
	double Comm_Set_alt;
	double Comm_Set_Freq_Bw;
	double Comm_Set_Freq;
	double Comm_Set_T;
	double Comm_Set_Power;

};

struct Comm_Simple_para {
	
	
	
	
	double Comm_Set_G;
	double Comm_Set_Freq_Bw;
	double Comm_Set_Freq;
	double Comm_Set_T;
	double Comm_Set_Power;

};

struct Comm_Complex_para {
	
	
	double Comm_Set_azi;
	double Comm_Set_alt;
	double Comm_Set_Freq_Bw;
	double Comm_Set_Freq;
	double Comm_Set_T;
	double Comm_Set_Power;

};

struct facilityPOS {
	double Comm_Set_longitude;
	double Comm_Set_latitude;
	double Comm_Set_altitude;
};

struct carrier {
	double psi;
	double thelta;
	double phi;
};

struct Env {
	int Rain;
	int Atmosphere;
	int Cloud;
};

struct Atmosphere {
	double oxygen;
	double steam;
};

struct relative_POS {
	double azimuth;
	double altitude;
	double d;

};





}