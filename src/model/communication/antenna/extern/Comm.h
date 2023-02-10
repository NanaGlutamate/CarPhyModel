#pragma once
//#include <iostream>
#include <unordered_map>
#include <cmath>
// #include"tinystr.h"
// #include"tinyxml.h"
//#include "armadillo"
//#include "csmodel_base.h"
//#include "models_data_types.h"
#include <any>
#include "para.h"

namespace externModel::comm{

class Comm
{
public:
	Comm();
	~Comm();
	bool Init();
	bool tran_Init(const Fixed_para &tran);
	bool rec_Init(const Fixed_para& rec);
	bool Comm_Init(Comm* comm1);
	bool Tick(double time);
	bool SetInput(const facilityPOS& postion, const Env& environment, const double& jam,const carrier& vehicle);
	facilityPOS Get_tranpos();
	double Get_tranp();
	double Get_trangt();
	double Get_trangtf();
	double Get_trangB();
	std::unordered_map<double, bool> GetBER();
	void Get();
	Comm *Comm_tran;
	double Getber();
	bool Getconnect();

	bool Comm_test(double Posa_x, double Posa_y, double Posa_z, 
		double Posb_x, double Posb_y, double Posb_z, double FT_comm);

private:
	Fixed_para tran_para;//发射机参数
	Fixed_para rec_para;//接收机参数
	facilityPOS comm_pos;//设备位置
	facilityPOS tran_pos;//要接收信号的位置
	carrier platform;//载体的欧拉角
	double Gt;
	double Gr;
	double G;
	Env env;
	double NJ;//干扰噪声
	double Nt;//环境噪声
	double N0;//总噪声
	relative_POS rel;//接收设备相对于发射设备的相对位置
	double ber;
	bool connect;
	std::unordered_map<double, bool> result;
	double Gain(double f,int i,double A);
	void tran_Gain();
	void rec_Gain();
	//void N_Jammer();//干扰噪声功率dB
	double N_R, N_C, N_A;//环境噪声
	void N_t();//环境噪声功率dB
	void N();//总的噪声功率dB
	void postion();
	void GAIN();//总的天线增益dB
	std::unordered_map<double, bool> BER();//误码率
	void Ber();
	//相对于地面坐标系的天线指向
	double azig;
	double altg;
	//大气吸收模型计算
	double Atmos();
	//大气折射效应
	double refraction();
};

}