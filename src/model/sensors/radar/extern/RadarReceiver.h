#pragma once

#include "RadarTransmitter.h"

namespace externModel::radar {

	class RadarReceiver
	{
	public:
		RadarReceiver();
		~RadarReceiver();

		void Init(RadarParams p_Params);

		double B; 
		double Nf; 
		double T0; 

		RadarTransmitter m_Trans;

		double CalNPower();	
	};

}