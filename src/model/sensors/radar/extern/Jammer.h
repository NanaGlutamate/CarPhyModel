#pragma once
#include "dataType.hpp"
#include "Antenna.h"


#include <string>
#include <any>
#include <map>
#include <unordered_map>

namespace externModel::radar {

	class Jammer
	{
	public:
		Jammer();
		~Jammer();

	private:
		double Frequency;	
		double FreqRange;	 
		double EffRadarPower;	
		double BeamAzSweep;	
		double BeamElSweep;	
		double BeamAzCenter;	
		double BeamElCenter;	
		double BeamSweepSync; 

		
		Vector3 JPos;
		Vector3 JVel;
		
		
		
		
		
		

		double P_j;		
		double Gt_sita;	
		double Lamda;   
		double Gama_j;	
		double R_j;		
		double LL;		
		bool sign;      

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