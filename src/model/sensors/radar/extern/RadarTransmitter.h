#pragma once

#include "dataType.hpp"

namespace externModel::radar {

	class RadarTransmitter
	{
	public:
		RadarTransmitter();
		~RadarTransmitter();
		void Init(RadarParams p_Params);

		double Dp;	//���书�� �㶨ʱ��PD�״�ľ�����ʧϵ��
		double Dav;	//ƽ�����书�ʺ㶨ʱ��PD�״�ľ�����ʧϵ��
		double Tpulse;		//�������
		double Tgate;		//���ſ���
		double PRF;		//�����ظ�Ƶ��
		int NGates;		//���ڷ�������֮��ĵȿ����벨����
		int NFilters;			//ÿ���Ŷ������˲�����Ŀ
		int M;		//Ŀ���Ӧ�ľ����ź�
		int N;		//Ŀ���Ӧ�Ķ������˲�����
		double Ru;	//��ģ��̽�����
		bool bVisible; //Ŀ��ɼ���
		bool Power; //�״￪��״̬
		double Pt; //�״��ֵ���� W
		double Pav; //�״�ƽ������ W
		double Frequency; //�״���ƵHz
		double PulseWidth; //�״����� S
		double lamda; //�״��źŲ��� m
		double D; //ϵͳ���� dB


		double CalDav();	//���������ʧϵ��
	};

}