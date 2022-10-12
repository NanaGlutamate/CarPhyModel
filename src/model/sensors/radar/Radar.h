#pragma once
#include "CoordTransform.hpp"
#include <vector>
#include "RadarReceiver.h"
#include "Jammer.h"
#include "Clutter.h"
#include "RadarTransmitter.h"

namespace radar {

	class Radar
	{
	public:

		bool Init();
		bool InitValue(const std::string& name, void* value);
		bool SetInput(const std::unordered_map<std::string, std::any>& value);
		void update();
		//设置干扰机功率
		void Set_JamPower(double JamP);
		void Set_radar_state(Vector3& Pos, Vector3& Vel);
		string Get_detect_result();
	//private:
		double SystemLoss;			//dB
		double SNRThreshold;		//信噪比阈值
		double MaxDetectRange;		//参数决定的最大斜距
		double Phi;					//坐标转换用
		double MaxDetectAngle;		//[无人机雷达]参数决定的最大探测角度
		double CJam;                //受干扰的功率
		Antenna m_Antenna;
		Clutter m_Clutter;
		//Jammer m_Jammer;
		RadarTransmitter m_Transmitter;
		RadarReceiver m_Receiver;
		TemporaryDetectSwitch m_Switch;
		RADARSTATE ENURadarState;
		TARGETSTATE ENUTargetState;
		RadarParams p_Params;

	};

}