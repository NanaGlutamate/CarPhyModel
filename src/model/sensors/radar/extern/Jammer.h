#pragma once
#include "dataType.hpp"
#include "Antenna.h"
#include"tinystr.h"
#include"tinyxml.h"
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
		double Frequency;	//���Ż�Ƶ��
		double FreqRange;	 //Ƶ�ʷ�Χ
		double EffRadarPower;	//��Ч���书��
		double BeamAzSweep;	//������λ����
		double BeamElSweep;	//������������
		double BeamAzCenter;	//������λ���Ľ�
		double BeamElCenter;	//�����������Ľ�
		double BeamSweepSync; //�԰��ƽ

		//���β���
		Vector3 JPos;
		Vector3 JVel;
		//double Lx;	//X����
		//double Ly;	//Y����
		//double Lz;	//Z����
		//double Vx;	//X�����ٶ�
		//double Vy;	//Y�����ٶ�
		//double Vz;	//Z�����ٶ�

		double P_j;		//Ϊ���Ż���Ч���书��
		double Gt_sita;	//Ϊ�״������ڸ��ŷ��������
		double Lamda;   //�״ﲨ��
		double Gama_j;	//Ϊ�����ź����״��źŵļ���ʧ����ʧϵ��
		double R_j;		//Ϊ�״������Դ�ľ���
		double LL;		//Ϊ��Ų����������������ص���ʧϵ��
		bool sign;      //�Ƿ���ŵ�

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