#pragma once
#include "Antenna.h"
#include "RadarTransmitter.h"
#include "windows.h"
#include <string>

using namespace std;

namespace radar {

    class Clutter {
    public:
        Clutter();
        ~Clutter();
        double AircraftAlt, AircraftVel, AntennaAzi, AntennaEl, AntennaBw;
        double RangeMAX, Sig0DB;
        double Freq;

        double AntennaBWR, Anti, Antj, Antk, RRes, RUnamb;
        double RangeDoppler[100][128];
        double Lambda;	//波长
        double DAZ;	//方位角步长
        double Gama;	//后向散射系数
        double Sig0;	//后向散射系数
        double FilterBW;	//多普勒滤波器带宽
        double GMax;	//天线最大方向增益

        double Range, AZ;	//用于循环迭代的距离、方位角
        int M;	//距离门号
        int N;	//多普勒滤波器号
        double EL;	//擦地角
        double CEL;	//擦地角余弦
        double Inti, Intj, Intk;	//杂波单元对应的方向余弦

        double DEL;	//增益的衰减
        double GANT; //计算杂波单元方向增益
        double SigC;	//杂波单元的RCS
        double m_Clutter;	//单元杂波
        double RK;	//计算常数

        double Doppler;	//杂波单元的多普勒频移
        double Lsys;	//系统损耗 dB

        RadarTransmitter m_Trans;
        Antenna m_Ant;

        HINSTANCE hlandclutter;
        HINSTANCE hseaclutter;

        void Init(RadarParams p_Params);

        //预处理
        void PreProcess();
        //求余
    //	double AMOD(double A,double B);
        //计算与主瓣中心夹角为DEL方向的天线增益因子
        double Gain(double DEL, double AntennaBWR);
        //计算M1-N1单元的杂波功率
        double CalClutterPower(int M1, int N1);

        double CPower(RADARSTATE p_RdState, TARGETSTATE p_TarState,
            RADARSTATE p_NUERdState, TARGETSTATE p_NUETarState);
        double EnvironmentClutterCalc(RADARSTATE p_NUERdState,
            TARGETSTATE p_NUETarState);
        double AMOD(double A, double B);
        string GetAppPath();
    };

}