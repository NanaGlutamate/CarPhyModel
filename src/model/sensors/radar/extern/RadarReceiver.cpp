#include "RadarReceiver.h"
#include <math.h>

namespace externModel::radar {

	RadarReceiver::RadarReceiver()
	{

	}

	RadarReceiver::~RadarReceiver()
	{

	}

	void RadarReceiver::Init(RadarParams p_Params)
	{
		m_Trans.Init(p_Params);
		//B = m_Trans.PRF / m_Trans.NFilters;
		B = p_Params.fre_band;
		Nf = 10;
		T0 = 290;
	}

	///DB
	double RadarReceiver::CalNPower()
	{
		double N;
		N = 10 * log10((4e-21) * B) + Nf;
		return N;
	}

}