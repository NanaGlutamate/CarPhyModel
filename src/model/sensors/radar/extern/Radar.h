#pragma once
#include "CoordTransform.hpp"
#include <vector>
#include "RadarReceiver.h"
#include "Jammer.h"
#include "Clutter.h"
#include "RadarTransmitter.h"

namespace externModel::radar {

	class Radar
	{
	public:

		bool Init();
		bool InitValue(const std::string& name, void* value);
		bool SetInput(const std::unordered_map<std::string, std::any>& value);
		void update();
		
		void Set_JamPower(double JamP);
		void Set_radar_state(Vector3& Pos, Vector3& Vel);
		string Get_detect_result();
	
		double SystemLoss;			
		double SNRThreshold;		
		double MaxDetectRange;		
		double Phi;					
		double MaxDetectAngle;		
		double CJam;                
		Antenna m_Antenna;
		Clutter m_Clutter;
		
		RadarTransmitter m_Transmitter;
		RadarReceiver m_Receiver;
		TemporaryDetectSwitch m_Switch;
		RADARSTATE ENURadarState;
		TARGETSTATE ENUTargetState;
		RadarParams p_Params;

	};

}