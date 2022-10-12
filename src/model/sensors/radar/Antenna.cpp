#include "Antenna.h"

namespace radar {

	Antenna::Antenna()
	{

	}

	Antenna::~Antenna()
	{

	}

	void Antenna::Init(RadarParams p_Params)
	{
		ScanAzRange = p_Params.ScanAzRange * DEG2RAD;
		ScanElRangeUl = p_Params.ScanElRangeUl * DEG2RAD;
		ScanElRangeLl = p_Params.ScanElRangeLl * DEG2RAD;
		ThetaAZ05 = p_Params.ThetaAZ05;
		ThetaEL05 = p_Params.ThetaEL05;
		ScanPeriod = p_Params.ScanPeriod;
		CalGml();
	}

	bool Antenna::bInConeAngle(TARGETSTATE p_TarState)
	{
		//计算天线波束的方位角[-PI,PI]、俯仰角[-PI/2,PI/2]
		AZ = std::atan2(p_TarState.tPos.x, p_TarState.tPos.y);
		EL = -std::atan2(p_TarState.tPos.z, sqrt(p_TarState.tPos.x * p_TarState.tPos.x + p_TarState.tPos.y * p_TarState.tPos.y));

		if ((fabs(AZ) <= ScanAzRange) && (EL <= ScanElRangeUl) && (EL >= ScanElRangeLl))
			return true;
		else
			return false;
	}

	void Antenna::CalAntAz(unsigned long TL)
	{

	}

	void Antenna::CalGml()
	{
		Gml = 4 * PI / (ThetaAZ05 * ThetaEL05 * DEG2RAD * DEG2RAD);
		Gml = 10 * log10(Gml);
		return;
	}

	//计算视在多普勒频率
	double Antenna::CalDopplerFrequency(RADARSTATE p_RdState, TARGETSTATE p_TarState)
	{
		double Vr, fd;
		Vr = p_RdState.rVel.x * cos(AZ) * cos(EL) - (p_TarState.tVel.x * cos(AZ) * cos(EL) + p_TarState.tVel.y * cos(EL) * sin(AZ) - p_TarState.tVel.z * sin(EL));
		fd = 2 * Vr / m_Trans.lamda;
		return fd;
	}

}