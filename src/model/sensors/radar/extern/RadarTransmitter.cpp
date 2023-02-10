#include "RadarTransmitter.h"

namespace externModel::radar {

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
		
		
		
		
		Pav = p_Params.Pt * Tpulse * PRF;
	}

	double RadarTransmitter::CalDav()	
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