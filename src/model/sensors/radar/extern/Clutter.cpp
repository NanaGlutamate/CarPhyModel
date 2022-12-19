#include "Clutter.h"

namespace externModel::radar {

    Clutter::Clutter() {

    }

    Clutter::~Clutter() {

    }

    void Clutter::Init(RadarParams p_Params) {
        m_Ant.Init(p_Params);
        m_Trans.Init(p_Params);
        Lsys = 4;	//dB

        //20191101 GH �غ��Ӳ�dll����
        string landclutteDir = GetAppPath() + "\\landclutter.dll";
        string seaclutteDir = GetAppPath() + "\\seaclutter.dll";
        hlandclutter = LoadLibraryEx(TEXT("D:\\landclutter.dll"), NULL,
            LOAD_WITH_ALTERED_SEARCH_PATH);
        hseaclutter = LoadLibraryEx(TEXT("D:\\seaclutter.dll"), NULL,
            LOAD_WITH_ALTERED_SEARCH_PATH);

        if (NULL == hlandclutter || NULL == hseaclutter) {
            //	AfxMessageBox(_T("�޷�load�Ӳ���"), MB_OK | MB_ICONSTOP);
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

        //�������߲����Ĺ�һ������ ����������ϵ
        Anti = cos(AntennaEl * DEG2RAD) * cos(AntennaAzi * DEG2RAD);
        Antj = cos(AntennaEl * DEG2RAD) * sin(AntennaAzi * DEG2RAD);
        Antk = -sin(AntennaEl * DEG2RAD);

        RUnamb = C / (2 * m_Trans.PRF);
        RRes = RUnamb / m_Trans.NGates;
    }

    double Clutter::Gain(double DEL, double AntennaBWR) {
        double Gt = 0.0;
        //DEL = DEL * PI/180; //�ӽǶ�ת��Ϊ����
        //���¼��㷽ʽͬ  ���Ż����������������
        double HalfPow_sita = AntennaBWR / 180. * PI;//�빦�ʿ���(����1-2��)
        double sing_lamda = 4.0 * PI / (3.0 * HalfPow_sita);//singa������ϵ��lamda
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
        //3��ģ������֮�ڼ����Ӳ�
        RangeMAX = 3 * RUnamb + AircraftAlt;
        Lambda = C / Freq;
        RK = pow(Lambda, 2) * m_Trans.Pav * m_Trans.Dav / (pow(4 * PI, 3) * pow(10,
            Lsys / 10));	//�����ֵ����100KW
        DAZ = 0.002;
        Gama = pow(10, Sig0DB / 10);  //��Sig0DB�ĵ�λת��Ϊ��
        FilterBW = m_Trans.PRF / m_Trans.NFilters;
        GMax = 4 * PI / (AntennaBWR * AntennaBWR);


        Range = RangeMAX;
        M = (int)(AMOD(RangeMAX, RUnamb) / RRes);

        EL = -asin(AircraftAlt / Range);
        Sig0 = Gama * AircraftAlt / Range;    //????

        while (Range > AircraftAlt + RRes / 2) {
            AZ = PI / 2;

            while ((AZ >= 0) && (M == M1)) {
                //�����Ӳ���Ԫ�����߹�һ������
                CEL = cos(EL);
                Inti = CEL * cos(AZ);
                Intj = CEL * sin(AZ);
                Intk = -sin(EL);

                //�������߲������Ӳ���Ԫ����֮��ļн�
                DEL = acos(Inti * Anti + Intj * Antj + Intk * Antk);

                //�����Ӳ���Ԫ�������������
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

        //		���RangeDoppler�������á�����ʱ�伫�������ó���ʵʱ
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
        //�б�Ŀ���Ӧ�Ĳ���-�����յ�Ԫ��
        double R, Rres, Fres;
        R = p_TarState.tPos.magnitude();
        m_Trans.Ru = 3e8 / (2 * m_Trans.PRF);
        Rres = m_Trans.Ru / m_Trans.NGates;
        M = (int)(AMOD(R, m_Trans.Ru) / Rres);
        Fres = m_Trans.PRF / m_Trans.NFilters;
        N = (int)(AMOD(m_Ant.CalDopplerFrequency(p_RdState, p_TarState),
            m_Trans.PRF) / Fres);

        //����þ����ţ������յ�Ԫ�ĸ����Ӳ�����
        AircraftAlt = p_RdState.Height;
        AircraftVel = p_RdState.rVel.x;
        AntennaAzi = m_Ant.AZ;
        AntennaEl = m_Ant.EL * RAD2DEG;
        AntennaBw = (m_Ant.ThetaAZ05 + m_Ant.ThetaEL05) / 2;
        Sig0DB = EnvironmentClutterCalc(p_NUERdState, p_NUETarState);
        Freq = m_Trans.Frequency;

        double ClutterP = CalClutterPower(M, N);

        //���������Ӳ�����
        double rx = p_NUETarState.tPos.x - p_NUERdState.rPos.x;
        double ry = p_NUETarState.tPos.y - p_NUERdState.rPos.y;
        double rz = p_NUETarState.tPos.z - p_NUERdState.rPos.z;
        //�״���Ŀ��ľ���
        double r = sqrt(rx * rx + ry * ry + rz * rz);
        //���������ؽ�
        double theta = asin(ry / r);
        if (theta == 0) theta = 10e-16;
        //�״�������ĳ���(��������������濴ΪĿ��)
        double R2Surface = fabs(p_NUERdState.rPos.y / sin(theta));

        if (theta < 0) {
            double x1 = p_NUERdState.rPos.y / tan(fabs(theta) - 0.5 * PI / 180.);
            double x2 = p_NUERdState.rPos.y / tan(fabs(theta) + 0.5 * PI / 180.);
            double longaxis = fabs(x1 - x2);
            double shortaxis = 2 * p_NUERdState.rPos.y / sin(fabs(theta)) * tan(
                0.5 * PI / 180.);
            double s = PI *
                longaxis * shortaxis;    //����������������հ빦�ʽǷ�Χ����

            double A1 = 10 * log10(m_Trans.Pav * pow(m_Trans.lamda, 2) * m_Trans.Dav)
                + 10 * log10(s) + Sig0DB + 2 * m_Ant.Gml;
            double B1 = 30 * log10(4 * PI) + 40 * log10(R2Surface) + Lsys;
            double mainBeamClutter = A1 - B1;  //dB

            //�������ϵ��
            //�״�����崮����
            double K = -65;    //����ϵ����dB

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

        typedef double(*lpComputefun)(int, int, int, double);//��
        lpComputefun Computefun;
        typedef double(*lpTSCfun)(int, int, int, float, float);//��
        lpTSCfun TSCfun;
        double currTime = 0;

        //�ж��ǵ�/��
        if (p_NUETarState.surfaceType == 0) {
            Computefun = (lpComputefun)GetProcAddress(hlandclutter, "compute");
            if (Computefun != NULL) {
                int a = 2; //����rock=0 Trees=1 grasses=2 shrub=3 shortvegetation=4 DrySnow=5 WetSnow=6
                int b = 4; //����(0=P���� 1=L���� 2=S���� 3=C���� 4=X���� 5=Ku���� 6=Ka����)
                int c = 0; //������ʽHH=0 HV=1 VV=2

                //������ؽ�
                double rx = p_NUETarState.tPos.x - p_NUERdState.rPos.x;
                double ry = p_NUETarState.tPos.y - p_NUERdState.rPos.y;
                double rz = p_NUETarState.tPos.z - p_NUERdState.rPos.z;

                //������ؽ�(0~90��)
                double tarDis = sqrt(rx * rx + ry * ry + rz * rz);
                double d = asin(ry / tarDis);
                // ASSERT(d<=0); 071226
                d = fabs(d) * 180.0 / PI;
                //double d = 45; //������ؽ�(0~90��)
                //�������ʼ��a-d��ֵ���ö����������ݸ�ֵ
                // ...
                sigma = Computefun(a, b, c, d);
            }
        }
        else if (p_NUETarState.surfaceType == 1) {
            TSCfun = (lpTSCfun)GetProcAddress(hseaclutter, "sigmaTSC");
            int a = 4; //����(0=P���� 1=L���� 2=S���� 3=C���� 4=X���� 5=Ku���� 6=Ka����)
            int b = 2; //����(0~5��)
            int c = 0; //����(HH=0,VV=1)

            //double d = 30.0; //���ؽ�(0.1~90��)
            //������ؽ�(0.1~90��)
            double rx = p_NUETarState.tPos.x - p_NUERdState.rPos.x;
            double ry = p_NUETarState.tPos.y - p_NUERdState.rPos.y;
            double rz = p_NUETarState.tPos.z - p_NUERdState.rPos.z;
            double tarDis = sqrt(rx * rx + ry * ry + rz * rz);
            double d = asin(ry / tarDis);
            // ASSERT(d<=0); 071226
            d = fabs(d) * 180.0 / PI;    //����->��

            double e = 0; //��λ��(0~180��)

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