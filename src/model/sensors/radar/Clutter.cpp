#include "Clutter.h"

namespace radar {

    Clutter::Clutter() {

    }

    Clutter::~Clutter() {

    }

    void Clutter::Init(RadarParams p_Params) {
        m_Ant.Init(p_Params);
        m_Trans.Init(p_Params);
        Lsys = 4;	//dB

        //20191101 GH 地海杂波dll加载
        string landclutteDir = GetAppPath() + "\\landclutter.dll";
        string seaclutteDir = GetAppPath() + "\\seaclutter.dll";
        hlandclutter = LoadLibraryEx(TEXT("D:\\landclutter.dll"), NULL,
            LOAD_WITH_ALTERED_SEARCH_PATH);
        hseaclutter = LoadLibraryEx(TEXT("D:\\seaclutter.dll"), NULL,
            LOAD_WITH_ALTERED_SEARCH_PATH);

        if (NULL == hlandclutter || NULL == hseaclutter) {
            //	AfxMessageBox(_T("无法load杂波库"), MB_OK | MB_ICONSTOP);
        }
    }

    string Clutter::GetAppPath() {
        //TCHAR modulePath[MAX_PATH];
        //GetModuleFileName(NULL, modulePath, MAX_PATH);
        //string strModulePath(modulePath);
        //strModulePath.resize(strModulePath.find_last_of('\\'));
        //return strModulePath;
        return " ";
    }

    void Clutter::PreProcess() {
        int i, j;
        for (i = 0; i < m_Trans.NGates; i++)
            for (j = 0; j < m_Trans.NFilters; j++)
                RangeDoppler[i][j] = 1e-16;

        AntennaBWR = AntennaBw * DEG2RAD;

        //计算天线波束的归一化向量 北东地坐标系
        Anti = cos(AntennaEl * DEG2RAD) * cos(AntennaAzi * DEG2RAD);
        Antj = cos(AntennaEl * DEG2RAD) * sin(AntennaAzi * DEG2RAD);
        Antk = -sin(AntennaEl * DEG2RAD);

        RUnamb = C / (2 * m_Trans.PRF);
        RRes = RUnamb / m_Trans.NGates;
    }

    double Clutter::Gain(double DEL, double AntennaBWR) {
        double Gt = 0.0;
        //DEL = DEL * PI/180; //从角度转化为弧度
        //以下计算方式同  干扰机接收天线增益计算
        double HalfPow_sita = AntennaBWR / 180. * PI;//半功率宽度(正常1-2度)
        double sing_lamda = 4.0 * PI / (3.0 * HalfPow_sita);//singa函数的系数lamda
        DEL = fabs(DEL);

        if (DEL <= 1e-15) {
            Gt = 1.0;
        }
        else if (DEL <= 0.75 / 180 * PI) {
            Gt = 1.0 * fabs(sin(sing_lamda * DEL) / (sing_lamda * DEL));
        }
        else if (DEL <= 0.75 / 180 * PI * 2.0) {
            Gt = 1.0 * (1e-4) * fabs(sin(3.0 * PI / 2.0) / (3.0 * PI / 2.0));
        }
        else
            Gt = 1.0 * (1e-7);
        //added by pj 080111}}

        return Gt;
    }

    //int main(int argc, char* argv[])
    double Clutter::CalClutterPower(int M1, int N1) {
        int i, j;


        PreProcess();
        //3倍模糊距离之内计算杂波
        RangeMAX = 3 * RUnamb + AircraftAlt;
        Lambda = C / Freq;
        RK = pow(Lambda, 2) * m_Trans.Pav * m_Trans.Dav / (pow(4 * PI, 3) * pow(10,
            Lsys / 10));	//假设峰值功率100KW
        DAZ = 0.002;
        Gama = pow(10, Sig0DB / 10);  //将Sig0DB的单位转化为瓦
        FilterBW = m_Trans.PRF / m_Trans.NFilters;
        GMax = 4 * PI / (AntennaBWR * AntennaBWR);


        Range = RangeMAX;
        M = (int)(AMOD(RangeMAX, RUnamb) / RRes);

        EL = -asin(AircraftAlt / Range);
        Sig0 = Gama * AircraftAlt / Range;    //????

        while (Range > AircraftAlt + RRes / 2) {
            AZ = PI / 2;

            while ((AZ >= 0) && (M == M1)) {
                //计算杂波单元的视线归一化向量
                CEL = cos(EL);
                Inti = CEL * cos(AZ);
                Intj = CEL * sin(AZ);
                Intk = -sin(EL);

                //计算天线波束与杂波单元视线之间的夹角
                DEL = acos(Inti * Anti + Intj * Antj + Intk * Antk);

                //计算杂波单元方向的天线增益
                GANT = Gain(DEL, AntennaBWR) * GMax;
                //GANT = 1.0*GMax;
                SigC = RRes * Range * DAZ * Sig0 / CEL;
                m_Clutter = pow(GANT, 2) * SigC * RK / pow(Range, 4);
                //Clutter = pow(GANT,2)*SigC/pow(Range,4);

                Doppler = AMOD(2 * (AircraftVel / Lambda) * Inti, m_Trans.PRF);
                N = int(Doppler / FilterBW);

                //printf("\n M = %d, N = %d, Range = %f",M,N,Range);
                RangeDoppler[M][N] = RangeDoppler[M][N] + m_Clutter;
                AZ = AZ - DAZ;
            }
            Range = Range - RRes;
            M = M - 1;
            if (M < 0) {
                M = m_Trans.NGates - 1;
            }

            EL = -asin(AircraftAlt / Range);
            Sig0 = Gama * AircraftAlt / Range;
        }

        for (i = 0; i < m_Trans.NGates; i++)
            for (j = 0; j < m_Trans.NFilters; j++)
                RangeDoppler[i][j] = 10 * log10(RangeDoppler[i][j]);

        //		输出RangeDoppler，【慎用】运行时间极长，会让程序不实时
        // 		FILE *fp;
        // 		fp = fopen("Result.m","w");
        // 		for(i=0; i<NRangeGates; i++)
        // 		{
        // 			for (j =0; j<NFilters; j++)
        // 				fprintf(fp,"%f  ",RangeDoppler[i][j]);
        // 			fprintf(fp,"\n");
        // 		}
        // 		fclose(fp);

        return RangeDoppler[M1][N1];
    }

    double Clutter::CPower(RADARSTATE p_RdState, TARGETSTATE p_TarState,
        RADARSTATE p_NUERdState, TARGETSTATE p_NUETarState) {
        //TODO
        //判别目标对应的波门-多普勒单元号
        double R, Rres, Fres;
        R = p_TarState.tPos.magnitude();
        m_Trans.Ru = 3e8 / (2 * m_Trans.PRF);
        Rres = m_Trans.Ru / m_Trans.NGates;
        M = (int)(AMOD(R, m_Trans.Ru) / Rres);
        Fres = m_Trans.PRF / m_Trans.NFilters;
        N = (int)(AMOD(m_Ant.CalDopplerFrequency(p_RdState, p_TarState),
            m_Trans.PRF) / Fres);

        //解算该距离门－多普勒单元的副瓣杂波功率
        AircraftAlt = p_RdState.Height;
        AircraftVel = p_RdState.rVel.x;
        AntennaAzi = m_Ant.AZ;
        AntennaEl = m_Ant.EL * RAD2DEG;
        AntennaBw = (m_Ant.ThetaAZ05 + m_Ant.ThetaEL05) / 2;
        Sig0DB = EnvironmentClutterCalc(p_NUERdState, p_NUETarState);
        Freq = m_Trans.Frequency;

        double ClutterP = CalClutterPower(M, N);

        //计算主瓣杂波功率
        double rx = p_NUETarState.tPos.x - p_NUERdState.rPos.x;
        double ry = p_NUETarState.tPos.y - p_NUERdState.rPos.y;
        double rz = p_NUETarState.tPos.z - p_NUERdState.rPos.z;
        //雷达与目标的距离
        double r = sqrt(rx * rx + ry * ry + rz * rz);
        //主波束擦地角
        double theta = asin(ry / r);
        if (theta == 0) theta = 10e-16;
        //雷达距离地面的长度(将主波束照射地面看为目标)
        double R2Surface = fabs(p_NUERdState.rPos.y / sin(theta));

        if (theta < 0) {
            double x1 = p_NUERdState.rPos.y / tan(fabs(theta) - 0.5 * PI / 180.);
            double x2 = p_NUERdState.rPos.y / tan(fabs(theta) + 0.5 * PI / 180.);
            double longaxis = fabs(x1 - x2);
            double shortaxis = 2 * p_NUERdState.rPos.y / sin(fabs(theta)) * tan(
                0.5 * PI / 180.);
            double s = PI *
                longaxis * shortaxis;    //主波束照面积。按照半功率角范围计算

            double A1 = 10 * log10(m_Trans.Pav * pow(m_Trans.lamda, 2) * m_Trans.Dav)
                + 10 * log10(s) + Sig0DB + 2 * m_Ant.Gml;
            double B1 = 30 * log10(4 * PI) + 40 * log10(R2Surface) + Lsys;
            double mainBeamClutter = A1 - B1;  //dB

            //计算改善系数
            //雷达的脉冲串个数
            double K = -65;    //改善系数。dB

            double pow_mainbeam = pow(10., mainBeamClutter / 10.);
            double pow_clu = pow(10., ClutterP / 10.);
            double pow_all = pow_mainbeam + pow_clu;
            ClutterP = K + 10 * log10(pow_all);
        }
        return ClutterP;
    }

    double Clutter::EnvironmentClutterCalc(RADARSTATE p_NUERdState,
        TARGETSTATE p_NUETarState) {
        double sigma = -22.0;

        typedef double(*lpComputefun)(int, int, int, double);//地
        lpComputefun Computefun;
        typedef double(*lpTSCfun)(int, int, int, float, float);//海
        lpTSCfun TSCfun;
        double currTime = 0;

        //判断是地/海
        if (p_NUETarState.surfaceType == 0) {
            Computefun = (lpComputefun)GetProcAddress(hlandclutter, "compute");
            if (Computefun != NULL) {
                int a = 2; //地形rock=0 Trees=1 grasses=2 shrub=3 shortvegetation=4 DrySnow=5 WetSnow=6
                int b = 4; //波段(0=P波段 1=L波段 2=S波段 3=C波段 4=X波段 5=Ku波段 6=Ka波段)
                int c = 0; //极化方式HH=0 HV=1 VV=2

                //计算擦地角
                double rx = p_NUETarState.tPos.x - p_NUERdState.rPos.x;
                double ry = p_NUETarState.tPos.y - p_NUERdState.rPos.y;
                double rz = p_NUETarState.tPos.z - p_NUERdState.rPos.z;

                //计算擦地角(0~90度)
                double tarDis = sqrt(rx * rx + ry * ry + rz * rz);
                double d = asin(ry / tarDis);
                // ASSERT(d<=0); 071226
                d = fabs(d) * 180.0 / PI;
                //double d = 45; //输入擦地角(0~90度)
                //把上面初始的a-d的值，用订购来的数据赋值
                // ...
                sigma = Computefun(a, b, c, d);
            }
        }
        else if (p_NUETarState.surfaceType == 1) {
            TSCfun = (lpTSCfun)GetProcAddress(hseaclutter, "sigmaTSC");
            int a = 4; //波段(0=P波段 1=L波段 2=S波段 3=C波段 4=X波段 5=Ku波段 6=Ka波段)
            int b = 2; //海况(0~5级)
            int c = 0; //极化(HH=0,VV=1)

            //double d = 30.0; //擦地角(0.1~90度)
            //计算擦地角(0.1~90度)
            double rx = p_NUETarState.tPos.x - p_NUERdState.rPos.x;
            double ry = p_NUETarState.tPos.y - p_NUERdState.rPos.y;
            double rz = p_NUETarState.tPos.z - p_NUERdState.rPos.z;
            double tarDis = sqrt(rx * rx + ry * ry + rz * rz);
            double d = asin(ry / tarDis);
            // ASSERT(d<=0); 071226
            d = fabs(d) * 180.0 / PI;    //弧度->度

            double e = 0; //方位角(0~180度)

            sigma = TSCfun(a, b, c, d, e);
        }

        return sigma;
    }


    double Clutter::AMOD(double A, double B) {
        double result;
        result = A - (int)(A / B) * B;
        return result;
    }

}