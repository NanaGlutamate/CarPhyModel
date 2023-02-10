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
        double Lambda;	
        double DAZ;	
        double Gama;	
        double Sig0;	
        double FilterBW;	
        double GMax;	

        double Range, AZ;	
        int M;	
        int N;	
        double EL;	
        double CEL;	
        double Inti, Intj, Intk;	

        double DEL;	
        double GANT; 
        double SigC;	
        double m_Clutter;	
        double RK;	

        double Doppler;	
        double Lsys;	

        RadarTransmitter m_Trans;
        Antenna m_Ant;

        HINSTANCE hlandclutter;
        HINSTANCE hseaclutter;

        void Init(RadarParams p_Params);

        
        void PreProcess();
        
    
        
        double Gain(double DEL, double AntennaBWR);
        
        double CalClutterPower(int M1, int N1);

        double CPower(RADARSTATE p_RdState, TARGETSTATE p_TarState,
            RADARSTATE p_NUERdState, TARGETSTATE p_NUETarState);
        double EnvironmentClutterCalc(RADARSTATE p_NUERdState,
            TARGETSTATE p_NUETarState);
        double AMOD(double A, double B);
        string GetAppPath();
    };

}