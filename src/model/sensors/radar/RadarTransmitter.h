#pragma once
#include "dataType.hpp"

namespace radar {

	class RadarTransmitter
	{
	public:
		RadarTransmitter();
		~RadarTransmitter();
		void Init(RadarParams p_Params);

		double Dp;	//发射功率 恒定时，PD雷达的距离损失系数
		double Dav;	//平均发射功率恒定时，PD雷达的距离损失系数
		double Tpulse;		//脉冲宽度
		double Tgate;		//波门宽度
		double PRF;		//脉冲重复频率
		int NGates;		//相邻发射脉冲之间的等宽距离波门数
		int NFilters;			//每波门多普勒滤波器数目
		int M;		//目标对应的距离门号
		int N;		//目标对应的多普勒滤波器号
		double Ru;	//不模糊探测距离
		bool bVisible; //目标可见性
		bool Power; //雷达开关状态
		double Pt; //雷达峰值功率 W
		double Pav; //雷达平均功率 W
		double Frequency; //雷达载频Hz
		double PulseWidth; //雷达脉宽 S
		double lamda; //雷达信号波长 m
		double D; //系统增益 dB


		double CalDav();	//计算距离损失系数
	};

}