#pragma once

#include "dataType.hpp"

namespace externModel::radar {

	class RadarTransmitter
	{
	public:
		RadarTransmitter();
		~RadarTransmitter();
		void Init(RadarParams p_Params);

		double Dp;	
		double Dav;	
		double Tpulse;		
		double Tgate;		
		double PRF;		
		int NGates;		
		int NFilters;			
		int M;		
		int N;		
		double Ru;	
		bool bVisible; 
		bool Power; 
		double Pt; 
		double Pav; 
		double Frequency; 
		double PulseWidth; 
		double lamda; 
		double D; 


		double CalDav();	
	};

}