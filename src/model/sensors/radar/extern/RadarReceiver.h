#pragma once

#include "RadarTransmitter.h"

namespace externModel::radar {

	class RadarReceiver
	{
	public:
		RadarReceiver();
		~RadarReceiver();

		void Init(RadarParams p_Params);

		double B; //�첨ǰ����ϵͳ���������� Hz
		double Nf; //ϵͳ����ϵ�� dB
		double T0; //���ջ������¶� K

		RadarTransmitter m_Trans;

		double CalNPower();	//������ջ���������
	};

}