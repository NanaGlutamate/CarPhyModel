#include "Radar.h"

#include <time.h>
#include <iostream>



namespace externModel::radar {

	bool Radar::Init()
	{
		p_Params.PRF = 20e3;
		p_Params.Tpulse = 6.4e-6;
		p_Params.Tgate = 6.4e-6;
		p_Params.NFilters = 128;
		p_Params.Frequency = 3.1e9;
		p_Params.fre_band = 0.2e6;
		p_Params.Pt = 10000;
		p_Params.ScanAzRange = 360;
		p_Params.ScanElRangeLl = 0.0;
		p_Params.ScanElRangeUl = 90.0;
		p_Params.ScanPeriod = 10;
		p_Params.ThetaAZ05 = 1.3;
		p_Params.ThetaEL05 = 1.3;

		m_Antenna.Init(p_Params);
		m_Transmitter.Init(p_Params);
		m_Clutter.Init(p_Params);
		m_Receiver.Init(p_Params);
		SystemLoss = 4;
		SNRThreshold = 1.0;
		MaxDetectRange = 10e7;
		MaxDetectAngle = 90 * DEG2RAD;
		Phi = 0.0;

		return true;
	}

	bool Radar::InitValue(const std::string& name, void* value)
	{
		
		if ("RPos" == name) {
			ENURadarState.rPos = *(Vector3*)value;
		}
		else if ("RVel" == name) {
			ENURadarState.rVel = *(Vector3*)value;
		}
		else if ("RFrequency" == name) {
			ENURadarState.fre_center = *(double*)value;
		}
		else if ("RFreqBand" == name) {
			ENURadarState.fre_band = *(double*)value;
		}
		return true;
	}

	bool Radar::SetInput(const std::unordered_map<std::string, std::any>& value)
	{
		
		try {
			auto param = value.find("Target_Position");
			if (param != value.end()) {

				ENUTargetState.tPos = std::any_cast<Vector3>(param->second);
			}
			param = value.find("Target_Velocity");
			if (param != value.end()) {
				ENUTargetState.tVel = std::any_cast<Vector3>(param->second);
			}
		}
		catch (...) {
			return false;
		}
		return true;
	}

	void Radar::update() {
		
		auto range = (ENUTargetState.tPos - ENURadarState.rPos).magnitude();
		if (range > MaxDetectRange) {
			ENUTargetState.detected = false;
		}
		else {
			double dl = std::acos(std::fabs(ENUTargetState.tPos.z - ENURadarState.rPos.z) / range);
			if (dl > MaxDetectAngle) {
				ENUTargetState.detected = false;
			}
			else {
				m_Switch.RD_HasJam = true;
				m_Switch.RD_HasClutter = false;
				m_Switch.RD_JammerNum = 0;
				double A1, B1;
				A1 = 10 * log10(m_Transmitter.Pav * pow(m_Transmitter.lamda,
					2) * 1 * m_Transmitter.Dav) + 2 * m_Antenna.Gml;
				B1 = 30 * log10(4 * PI) + SystemLoss;

				double C1, C2, C3, C4;
				if (m_Switch.RD_HasClutter)
					C2 = 0;
				else
					C2 = 0;
				C3 = pow(10.0, m_Receiver.CalNPower() / 10.0);					
				if (m_Switch.RD_HasJam)
					C4 = CJam; 
				else
					C4 = 0;
				if (ENUTargetState.tPos.z > ENURadarState.rPos.z)   
					C1 = 10.0 * log10(C2 + C3 + C4);        
				else
					C1 = m_Receiver.CalNPower();

				double DetectRange;
				DetectRange = pow(10, (A1 - B1 - C1 - 10 * log10(SNRThreshold)) /
					40);	

  
				double SNR2 = pow(10, (A1 - B1 - C1 - 10 * log10(pow(range, 4))) / 10);
				double bSNR2 = (pow(10, SNR2 / 10.)) / 1.7;
				double Pd2 = 0.0;
				if (bSNR2 >= 20)
					Pd2 = 1;
				else
					Pd2 = exp(bSNR2) / (1. + exp(bSNR2));

				

				
				srand(time(NULL));
				double randnum = ((double)rand()) / ((double)RAND_MAX);
				if (randnum <= Pd2 && range <= DetectRange)
					ENUTargetState.detected = true;
				else
					ENUTargetState.detected = false;
			}
		}
	}


	void Radar::Set_JamPower(double JamP)
	{
		CJam = JamP;
	}

	void Radar::Set_radar_state(Vector3& Pos, Vector3& Vel) {
		ENURadarState.rVel = Vel;
		ENURadarState.rPos = Pos;
		
		
	}

	string Radar::Get_detect_result()
	{
		if (ENUTargetState.detected)
		{
			return "Detected Success";
		}
		else
		{
			return "Detected Failure";
		}
	}

}
