#pragma once
#include "RadarTransmitter.h"
#include "dataType.hpp"
#include <cmath>

namespace externModel::radar {

    class Antenna {
    public:
        Antenna();
        ~Antenna();
        int Type;	//�������� 0��AEW��1��AI
        unsigned long TK; //����ʱ��
        double ScanPeriod;		//����ɨ������
        double ThetaAZ05;		//���߷�λ�빦�ʽ�
        double ThetaEL05;		//���߸����빦�ʽ�
        double ScanAzRange;		//����ɨ�跽λ�Ƕȷ�Χ������AEW��360�ȣ�����AI��ͨ��������60�ȷ�Χ�ڵĽ�
        double ScanElRangeUl;		//����ɨ�踩��������
        double ScanElRangeLl;		//����ɨ�踩��������
        double Gml; //������������ dB
        double Gsl; //�����԰����� dB
        double AZ;	//����ָ��λ��
        double EL;	//����ָ������

    public:

        RadarTransmitter m_Trans;

        void Init(RadarParams p_Params);
        bool bInConeAngle(TARGETSTATE p_TarState);//�Ƿ�������̽��׶����

        void CalAntAz(unsigned long TL);		//�������߷�λ�� ��
        void CalAntEl(unsigned long TL);		//�������߸����� ��
        void CalGml();
        double CalDopplerFrequency(RADARSTATE p_RdState, TARGETSTATE p_TarState);
        double CalTheta(RADARSTATE RadarState,
            TARGETSTATE TargetState);	//��������ָ����Ŀ�귽��֮��ļн�
        double CalGain(double
            theta);	//����������Ŀ�귽�������,thetaΪ������Ŀ��֮��ķ���н�
    };

}