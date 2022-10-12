#include "RadarTransmitter.h"

namespace radar {

	RadarTransmitter::RadarTransmitter()
	{

	}

	RadarTransmitter::~RadarTransmitter()
	{

	}

	void RadarTransmitter::Init(RadarParams p_Params)
	{
		PRF = p_Params.PRF;
		Tpulse = p_Params.Tpulse;
		Tgate = p_Params.Tgate;
		NGates = (int)((1 / PRF - Tpulse) / Tgate);
		NFilters = p_Params.NFilters;
		Ru = 3e8 / (2 * PRF);
		Dav = CalDav();

		Frequency = p_Params.Frequency;
		lamda = 3e8 / Frequency;
		Power = true;
		//峰值功率Pt = 10Kw
		//脉冲重复频率 20e3，脉冲宽度即为: plus_width = 1/20e3 = 5e-5
		//有效脉冲宽度 1e-6，所以占空比为: tplus_ratio = 1e-6/plus_width = 2e-2;
		//因此，平均功率Pav = Pt*tplus_ratio = 200W
		Pav = p_Params.Pt * Tpulse * PRF;
	}

	double RadarTransmitter::CalDav()	//计算距离损失系数
	{
		double temp = Tpulse / Tgate;
		if (temp <= 1)
			Dav = (temp / (NGates + temp)) * (NGates - (5 * NGates - 1) * temp / 12);
		if (temp <= 2)
			Dav = (1 / (NGates + temp)) * (NGates * (1 - temp / 3) - (NGates - 1) * temp * temp / 12);
		if (temp > 2)
			Dav = (1 / (NGates + temp)) * ((NGates - 4 / 3) * temp + 1);

		return Dav;
	}

}