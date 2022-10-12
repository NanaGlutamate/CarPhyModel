#pragma once
#include "RadarTransmitter.h"

namespace radar {

	class RadarReceiver
	{
	public:
		RadarReceiver();
		~RadarReceiver();

		void Init(RadarParams p_Params);

		double B; //检波前接收系统的噪声带宽 Hz
		double Nf; //系统噪声系数 dB
		double T0; //接收机噪声温度 K

		RadarTransmitter m_Trans;

		double CalNPower();	//计算接收机噪声功率
	};

}