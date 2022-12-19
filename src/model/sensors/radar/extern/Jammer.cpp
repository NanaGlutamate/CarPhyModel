#include "Jammer.h"
#include <string>
#include "tinyxml.h"
#include "Radar.h"
#include <time.h>
#include <iostream>
//#include <atlbase.h>
//#include <atlwin.h>

using namespace std;

namespace externModel::radar {

	Jammer::Jammer()
	{

	}

	Jammer::~Jammer()
	{

	}

	bool Jammer::Init()
	{
		//TO DO! ����·��
		// HMODULE hModuleInstance = _AtlBaseModule.GetModuleInstance();
		// char* Radar_file_name = (char*)malloc(1000 * sizeof(char));
		// char   DllPath[MAX_PATH] = { 0 };
		// GetModuleFileNameA(hModuleInstance, DllPath, _countof(DllPath));
		// char drive[_MAX_DRIVE];
		// char dir[_MAX_DIR];
		// char fname[_MAX_FNAME];
		// char ext[_MAX_EXT];
		// _splitpath_s(DllPath, drive, dir, fname, ext);
		// string  maneuver_file = drive + std::string(dir) + "Jammer_property.xml";
		// strcpy(Radar_file_name, maneuver_file.data());
		char* Radar_file_name = (char*)malloc(1000 * sizeof(char));
		string  maneuver_file = "D:/Desktop/workspace/12others/2022-04-01CarModelling/P0_carModel/src/config/Jammer_property.xml";
		strcpy(Radar_file_name, maneuver_file.data());
		TiXmlDocument doc(Radar_file_name);
		if (!doc.LoadFile())
			return 0;
		TiXmlHandle hDoc(&doc);
		TiXmlElement* pElem;//����һ��ָ��Ԫ�ص�ָ��
		pElem = hDoc.FirstChildElement().Element();//����ָ��ָ����ڵ㣻��ָ���ˡ�bluePADGroup"���
		//�״Ｐ���Ż��Ĳ���
		//p_Params.PRF = stod(pElem->FirstChildElement("Radar")->FirstChildElement("PRF")->GetText());
		//p_Params.Tpulse = stod(pElem->FirstChildElement("Radar")->FirstChildElement("Tpulse")->GetText());
		//p_Params.Tgate = stod(pElem->FirstChildElement("Radar")->FirstChildElement("Tgate")->GetText());
		//p_Params.NFilters = stod(pElem->FirstChildElement("Radar")->FirstChildElement("NFilters")->GetText());
		//p_Params.Frequency = stod(pElem->FirstChildElement("Radar")->FirstChildElement("fre_center")->GetText());
		//p_Params.fre_band = stod(pElem->FirstChildElement("Radar")->FirstChildElement("fre_band")->GetText());
		//p_Params.Pt = stod(pElem->FirstChildElement("Radar")->FirstChildElement("Pt")->GetText());
		p_Params.ScanAzRange = stod(pElem->FirstChildElement("Antenna")->FirstChildElement("ScanAzRange")->GetText());
		p_Params.ScanElRangeLl = stod(pElem->FirstChildElement("Antenna")->FirstChildElement("ScanElRangeLl")->GetText());
		p_Params.ScanElRangeUl = stod(pElem->FirstChildElement("Antenna")->FirstChildElement("ScanElRangeUl")->GetText());
		p_Params.ScanPeriod = stod(pElem->FirstChildElement("Antenna")->FirstChildElement("ScanPeriod")->GetText());
		p_Params.ThetaAZ05 = stod(pElem->FirstChildElement("Antenna")->FirstChildElement("ThetaAZ05")->GetText());
		p_Params.ThetaEL05 = stod(pElem->FirstChildElement("Antenna")->FirstChildElement("ThetaEL05")->GetText());
		BeamAzCenter = DEG2RAD * stod(pElem->FirstChildElement("Jammer")->FirstChildElement("BeamAzCenter")->GetText());
		BeamAzSweep = DEG2RAD * stod(pElem->FirstChildElement("Jammer")->FirstChildElement("BeamAzSweep")->GetText());
		BeamElCenter = DEG2RAD * stod(pElem->FirstChildElement("Jammer")->FirstChildElement("BeamElCenter")->GetText());
		BeamElSweep = DEG2RAD * stod(pElem->FirstChildElement("Jammer")->FirstChildElement("BeamElSweep")->GetText());
		Frequency = stod(pElem->FirstChildElement("Jammer")->FirstChildElement("Frequency")->GetText());
		FreqRange = stod(pElem->FirstChildElement("Jammer")->FirstChildElement("FreqRange")->GetText());
		P_j = stod(pElem->FirstChildElement("Jammer")->FirstChildElement("P_j")->GetText());
		m_Ant.Init(p_Params);
		//�Ը��Ż�����Ϣ���г�ʼ��
		Gama_j = 0.5;
		Gt_sita = 0;
		Lamda = 3e8 / Frequency;
		LL = 1;
		R_j = 1;
		sign = 0;
		//p_RdState.fre_band = p_Params.fre_band;
		//p_RdState.fre_center = p_Params.fre_center;
	}

	bool Jammer::InitValue(const std::string& name, void* value)
	{
		//���Ż�λ�á��ٶȡ����ŵ�Ƶ�ʺͷ�Χ
		if ("JPos" == name) {
			JPos = *(Vector3*)value;
		}
		else if ("JVel" == name) {
			JVel = *(Vector3*)value;
		}
		else if ("JFrequency" == name) {
			Frequency = *(double*)value;
		}
		else if ("JFreqRange" == name) {
			FreqRange = *(double*)value;
		}
		return true;
	}

	bool Jammer::SetInput(const std::unordered_map<std::string, std::any>& value)
	{
		//���ô����� �״�״�̽��Ŀ���λ�á��ٶ�
		try {
			auto param = value.find("Target_Position");
			if (param != value.end()) {

				p_TarState.tPos = std::any_cast<Vector3>(param->second);
			}
			param = value.find("Target_Velocity");
			if (param != value.end()) {
				p_TarState.tVel = std::any_cast<Vector3>(param->second);
			}
			param = value.find("Radar_Postion");
			if (param != value.end()) {
				p_RdState.rPos = std::any_cast<Vector3>(param->second);
			}
			param = value.find("Radar_Velocity");
			if (param != value.end()) {
				p_RdState.rVel = std::any_cast<Vector3>(param->second);
			}
			param = value.find("Radar_FreCenter");
			if (param != value.end()) {
				p_RdState.fre_center = std::any_cast<double>(param->second);
			}
			param = value.find("Radar_FreBand");
			if (param != value.end()) {
				p_RdState.fre_band = std::any_cast<double>(param->second);
			}
		}
		catch (...) {
			return false;
		}
		return true;
	}




	double Jammer::CalcJPower()
	{
		double jamPower = 0.0;
		bool isInFangwei = false;
		bool isInFuyang = false;
		double dong = p_RdState.rPos.z - JPos.z;
		double bei = p_RdState.rPos.x - JPos.x;
		double tian = p_RdState.rPos.y - JPos.y;
		double tmpsita = 0;
		double lxz = 0.0;    //��-��ƽ�����
		//added by pj 071217{{
		double nowdis = sqrt(dong * dong + bei * bei + tian * tian);
		//�����н�
		if (tian <= 0)
		{
			lxz = sqrt(bei * bei + dong * dong);
			tmpsita = atan(tian / lxz);
			if (fabs(tmpsita - BeamElCenter) <= (BeamElSweep / 2)) {
				isInFuyang = true;
			}
		}
		//��λ�Ǽн�. ���춫����
		if (isInFuyang)
		{
			tmpsita = 0;
			try {
				tmpsita = atan(fabs(dong / bei));
			}
			catch (...) {
			}
			if (dong >= 0 && bei >= 0) { //��һ����
			}
			else if (dong >= 0 && bei <= 0) { //�ڶ�����
				tmpsita = PI - tmpsita;
			}
			else if (dong <= 0 && bei <= 0) { //����
				tmpsita += PI;
			}
			else if (dong <= 0 && bei >= 0) { // ����
				tmpsita = 2 * PI - tmpsita;
			}
			if (fabs(tmpsita - BeamAzCenter) <= (BeamAzSweep / 2))
			{
				isInFangwei = true;
			}
		}
		//�ж��Ƿ��ڸ��Ż�ɨ��׵����
		sign = isInFangwei && isInFuyang;
		if (isInFangwei && isInFuyang)
		{
			//���Ŀ����̽��׶���ڣ����һ��ڸ��Ų������÷�Χ��
			//�ż�����Ż���ز���

			//���ܶȺ�������Gt_sita
			//Ϊ�״������ڸ��ŷ��������		
			double Gose_lib(double);

			//������Ų����״�����֮��ļн�
			Vector3 mt, mj;
			mt.x = p_TarState.tPos.x - p_RdState.rPos.x;
			mt.y = p_TarState.tPos.y - p_RdState.rPos.y;
			mt.z = p_TarState.tPos.z - p_RdState.rPos.z;
			mj.x = JPos.x - p_RdState.rPos.x;
			mj.y = JPos.y - p_RdState.rPos.y;
			mj.z = JPos.z - p_RdState.rPos.z;

			//���ݸ��Ų�������Ƕȼ��㸱������
			double tmp_d = sqrt(mt.x * mt.x + mt.y * mt.y + mt.z * mt.z) *
				sqrt(mj.x * mj.x + mj.y * mj.y + mj.z * mj.z);
			double tmp_vector = mt.x * mj.x + mt.y * mj.y + mt.z * mj.z;
			double alpha = acos(tmp_vector / tmp_d);

			//modefied by pj 071220 {{
			double HalfPow_sita = 1.0 / 180 * PI;//�빦�ʿ���(����1-2��)
			double sing_lamda = 4.0 * PI / (3.0 * HalfPow_sita);//singa������ϵ��lamda
			alpha = fabs(alpha);
			double AntennaGain = pow(10., m_Ant.Gml / 10.);
			//modefied by pj 071220 {{
			if (alpha <= 1e-15) {
				Gt_sita = AntennaGain;
			}
			else if (alpha <= 0.75 / 180 * PI) {
				Gt_sita = AntennaGain * fabs(sin(sing_lamda * alpha) / (sing_lamda * alpha));
			}
			else if (alpha <= 0.75 / 180 * PI * 2.0) {
				Gt_sita = AntennaGain * (1e-4) * fabs(sin(3.0 * PI / 2.0) / (3.0 * PI / 2.0));
			}
			else
				Gt_sita = AntennaGain * (1e-10);
			//modefied by pj 071220 }}
						//Ϊ��Ų����������������ص���ʧϵ��
			LL = 0.005;  //���� ��λ�������������� modified by pj 071220��

			Gama_j = 0.5;

			//Ϊ�״������Դ�ľ���
			R_j = sqrt(lxz * lxz + tian * tian);

			//���Ż�����;��λ����
			jamPower += PrjPower(p_RdState.fre_center, p_RdState.fre_band, Frequency, FreqRange);
		}
		return jamPower;
	}

	double Jammer::PrjPower(double fac, double fab, double fbc, double fbb)
	{
		double k = 1;
		double mi = (fac - fab / 2) > (fbc - fbb / 2) ? (fac - fab / 2) : (fbc - fbb / 2);
		double ma = (fac + fab / 2) < (fbc + fbb / 2) ? (fac + fab / 2) : (fbc + fbb / 2);
		ma = ma > mi ? ma : mi;

		try
		{
			k = fabs((ma - mi) / fbb);
		}
		catch (...) {
			k = 0;
			sign = false;
		}
		if (k < 1e-10)
		{
			sign = false;
		}
		double t_res = 0.0;

		try
		{
			double res = (k * P_j * Gt_sita * Lamda
				* Lamda * Gama_j
				/ (16 * PI * PI * R_j * R_j));
			t_res = 10 * log10(res);

			return res;
		}
		catch (...) {
			return 0;
		}
	}

	void Jammer::Set_BeamCenter(double& AzCenter, double& ElCenter)
	{
		BeamAzCenter = AzCenter * DEG2RAD;
		BeamElCenter = ElCenter * DEG2RAD;
	}

	void Jammer::Set_Freq(double& Freq, double& Freq_Range)
	{
		Frequency = Freq;
		FreqRange = Freq_Range;
	}

	void Jammer::Set_Pj(double& P)
	{
		P_j = P;
	}

	string Jammer::GetState(void)
	{
		if (sign)
		{
			return "Interference success";
		}
		else
		{
			return "Interference failure";
		}
	}

}