#pragma once
#include "RadarTransmitter.h"
#include "dataType.hpp"
#include <cmath>

namespace radar {

    class Antenna {
    public:
        Antenna();
        ~Antenna();
        int Type;	//天线类型 0－AEW；1－AI
        unsigned long TK; //开机时刻
        double ScanPeriod;		//天线扫描周期
        double ThetaAZ05;		//天线方位半功率角
        double ThetaEL05;		//天线俯仰半功率角
        double ScanAzRange;		//天线扫描方位角度范围，对于AEW是360度，对于AI则通常是正负60度范围内的角
        double ScanElRangeUl;		//天线扫描俯仰角上限
        double ScanElRangeLl;		//天线扫描俯仰角下限
        double Gml; //天线主瓣增益 dB
        double Gsl; //天线旁瓣增益 dB
        double AZ;	//天线指向方位角
        double EL;	//天线指向俯仰角

    public:

        RadarTransmitter m_Trans;

        void Init(RadarParams p_Params);
        bool bInConeAngle(TARGETSTATE p_TarState);//是否在天线探测锥体内

        void CalAntAz(unsigned long TL);		//计算天线方位角 度
        void CalAntEl(unsigned long TL);		//计算天线俯仰角 度
        void CalGml();
        double CalDopplerFrequency(RADARSTATE p_RdState, TARGETSTATE p_TarState);
        double CalTheta(RADARSTATE RadarState,
            TARGETSTATE TargetState);	//计算天线指向与目标方向之间的夹角
        double CalGain(double
            theta);	//计算天线在目标方向的增益,theta为天线与目标之间的方向夹角
    };

}