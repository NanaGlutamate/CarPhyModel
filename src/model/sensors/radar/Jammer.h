#pragma once
#include "dataType.hpp"
#include "Antenna.h"
#include"tinystr.h"
#include"tinyxml.h"
#include <string>
#include <any>
#include <map>
#include <unordered_map>

namespace radar {

	class Jammer
	{
	public:
		Jammer();
		~Jammer();

	private:
		double Frequency;	//干扰机频率
		double FreqRange;	 //频率范围
		double EffRadarPower;	//有效辐射功率
		double BeamAzSweep;	//波束方位宽度
		double BeamElSweep;	//波束俯仰宽度
		double BeamAzCenter;	//波束方位中心角
		double BeamElCenter;	//波束俯仰中心角
		double BeamSweepSync; //旁瓣电平

		//几何参数
		Vector3 JPos;
		Vector3 JVel;
		//double Lx;	//X坐标
		//double Ly;	//Y坐标
		//double Lz;	//Z坐标
		//double Vx;	//X方向速度
		//double Vy;	//Y方向速度
		//double Vz;	//Z方向速度

		double P_j;		//为干扰机等效发射功率
		double Gt_sita;	//为雷达天线在干扰方向的增益
		double Lamda;   //雷达波长
		double Gama_j;	//为干扰信号与雷达信号的极化失配损失系数
		double R_j;		//为雷达与干扰源的距离
		double LL;		//为电磁波因大气、降雨等因素的损失系数
		bool sign;      //是否干扰到

		Antenna m_Ant;
		RadarParams p_Params;
		RADARSTATE p_RdState;
		TARGETSTATE p_TarState;
		double PrjPower(double fac, double fab, double fbc, double fbb);

	public:
		bool Init();
		bool InitValue(const std::string& name, void* value);
		bool SetInput(const std::unordered_map<std::string, std::any>& value);
		double CalcJPower();
		void Set_BeamCenter(double& AzCenter, double& ElCenter);
		void Set_Freq(double& Freq, double& Freq_Range);
		void Set_Pj(double& P);
		std::string GetState(void);
	};

}