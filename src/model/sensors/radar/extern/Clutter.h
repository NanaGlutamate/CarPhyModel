#pragma once
#include "Antenna.h"
#include "RadarTransmitter.h"
#include "windows.h"
#include <string>

namespace externModel::radar {

using namespace std;

    class Clutter {
    public:
        Clutter();
        ~Clutter();
        double AircraftAlt, AircraftVel, AntennaAzi, AntennaEl, AntennaBw;
        double RangeMAX, Sig0DB;
        double Freq;

        double AntennaBWR, Anti, Antj, Antk, RRes, RUnamb;
        double RangeDoppler[100][128];
        double Lambda;	//����
        double DAZ;	//��λ�ǲ���
        double Gama;	//����ɢ��ϵ��
        double Sig0;	//����ɢ��ϵ��
        double FilterBW;	//�������˲�������
        double GMax;	//�������������

        double Range, AZ;	//����ѭ�������ľ��롢��λ��
        int M;	//�����ź�
        int N;	//�������˲�����
        double EL;	//���ؽ�
        double CEL;	//���ؽ�����
        double Inti, Intj, Intk;	//�Ӳ���Ԫ��Ӧ�ķ�������

        double DEL;	//�����˥��
        double GANT; //�����Ӳ���Ԫ��������
        double SigC;	//�Ӳ���Ԫ��RCS
        double m_Clutter;	//��Ԫ�Ӳ�
        double RK;	//���㳣��

        double Doppler;	//�Ӳ���Ԫ�Ķ�����Ƶ��
        double Lsys;	//ϵͳ��� dB

        RadarTransmitter m_Trans;
        Antenna m_Ant;

        HINSTANCE hlandclutter;
        HINSTANCE hseaclutter;

        void Init(RadarParams p_Params);

        //Ԥ����
        void PreProcess();
        //����
    //	double AMOD(double A,double B);
        //�������������ļн�ΪDEL�����������������
        double Gain(double DEL, double AntennaBWR);
        //����M1-N1��Ԫ���Ӳ�����
        double CalClutterPower(int M1, int N1);

        double CPower(RADARSTATE p_RdState, TARGETSTATE p_TarState,
            RADARSTATE p_NUERdState, TARGETSTATE p_NUETarState);
        double EnvironmentClutterCalc(RADARSTATE p_NUERdState,
            TARGETSTATE p_NUETarState);
        double AMOD(double A, double B);
        string GetAppPath();
    };

}