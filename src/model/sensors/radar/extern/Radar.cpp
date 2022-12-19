#include "Radar.h"
//#include "radar66.h"
#include <time.h>
#include <iostream>
//#include <atlbase.h>
//#include <atlwin.h>

namespace externModel::radar {

	bool Radar::Init()
	{
		//TO DO! ����·��
		/*HMODULE hModuleInstance = _AtlBaseModule.GetModuleInstance();*/
		char* Radar_file_name = (char*)malloc(1000 * sizeof(char));
		/*char DllPath[MAX_PATH] = { 0 };
		GetModuleFileNameA(hModuleInstance, DllPath, _countof(DllPath));
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath_s(DllPath, drive, dir, fname, ext);*/
		//TODO: reduce hard-coded path
		string  maneuver_file = "D:/Desktop/workspace/12others/2022-04-01CarModelling/P0_carModel/src/config/Jammer_property.xml";
		strcpy(Radar_file_name, maneuver_file.data());
		TiXmlDocument doc(Radar_file_name);
		if (!doc.LoadFile()){
			std::cout << "no file found";
			return false;
		}
		TiXmlHandle hDoc(&doc);
		TiXmlElement* pElem;//����һ��ָ��Ԫ�ص�ָ��
		pElem = hDoc.FirstChildElement().Element();//����ָ��ָ����ڵ㣻��ָ���ˡ�bluePADGroup"���
		//�������״�Ĳ���
		p_Params.PRF = stod(pElem->FirstChildElement("Radar")->FirstChildElement("PRF")->GetText());
		p_Params.Tpulse = stod(pElem->FirstChildElement("Radar")->FirstChildElement("Tpulse")->GetText());
		p_Params.Tgate = stod(pElem->FirstChildElement("Radar")->FirstChildElement("Tgate")->GetText());
		p_Params.NFilters = stod(pElem->FirstChildElement("Radar")->FirstChildElement("NFilters")->GetText());
		p_Params.Frequency = stod(pElem->FirstChildElement("Radar")->FirstChildElement("fre_center")->GetText());
		p_Params.fre_band = stod(pElem->FirstChildElement("Radar")->FirstChildElement("fre_band")->GetText());
		p_Params.Pt = stod(pElem->FirstChildElement("Radar")->FirstChildElement("Pt")->GetText());
		p_Params.ScanAzRange = stod(pElem->FirstChildElement("Antenna")->FirstChildElement("ScanAzRange")->GetText());
		p_Params.ScanElRangeLl = stod(pElem->FirstChildElement("Antenna")->FirstChildElement("ScanElRangeLl")->GetText());
		p_Params.ScanElRangeUl = stod(pElem->FirstChildElement("Antenna")->FirstChildElement("ScanElRangeUl")->GetText());
		p_Params.ScanPeriod = stod(pElem->FirstChildElement("Antenna")->FirstChildElement("ScanPeriod")->GetText());
		p_Params.ThetaAZ05 = stod(pElem->FirstChildElement("Antenna")->FirstChildElement("ThetaAZ05")->GetText());
		p_Params.ThetaEL05 = stod(pElem->FirstChildElement("Antenna")->FirstChildElement("ThetaEL05")->GetText());
		m_Antenna.Init(p_Params);
		m_Transmitter.Init(p_Params);
		m_Clutter.Init(p_Params);
		m_Receiver.Init(p_Params);
		SystemLoss = 4;
		SNRThreshold = 1.0;
		MaxDetectRange = 10e7;
		MaxDetectAngle = 90 * DEG2RAD;
		Phi = 0.0;
		return true;
	}

	bool Radar::InitValue(const std::string& name, void* value)
	{
		//�״�λ�á��ٶȡ�Ƶ�ʺʹ���
		if ("RPos" == name) {
			ENURadarState.rPos = *(Vector3*)value;
		}
		else if ("RVel" == name) {
			ENURadarState.rVel = *(Vector3*)value;
		}
		else if ("RFrequency" == name) {
			ENURadarState.fre_center = *(double*)value;
		}
		else if ("RFreqBand" == name) {
			ENURadarState.fre_band = *(double*)value;
		}
		return true;
	}

	bool Radar::SetInput(const std::unordered_map<std::string, std::any>& value)
	{
		//���ô�̽��Ŀ���λ�á��ٶ�
		try {
			auto param = value.find("Target_Position");
			if (param != value.end()) {

				ENUTargetState.tPos = std::any_cast<Vector3>(param->second);
			}
			param = value.find("Target_Velocity");
			if (param != value.end()) {
				ENUTargetState.tVel = std::any_cast<Vector3>(param->second);
			}
		}
		catch (...) {
			return false;
		}
		return true;
	}

	void Radar::update() {
		//Set_radar_state();
		auto range = (ENUTargetState.tPos - ENURadarState.rPos).magnitude();
		if (range > MaxDetectRange) {
			ENUTargetState.detected = false;
		}
		else {
			double dl = std::acos(std::fabs(ENUTargetState.tPos.z - ENURadarState.rPos.z) / range);
			if (dl > MaxDetectAngle) {
				ENUTargetState.detected = false;
			}
			else {
				m_Switch.RD_HasJam = true;
				m_Switch.RD_HasClutter = false;
				m_Switch.RD_JammerNum = 0;
				double A1, B1;
				A1 = 10 * log10(m_Transmitter.Pav * pow(m_Transmitter.lamda,
					2) * 1 * m_Transmitter.Dav) + 2 * m_Antenna.Gml;
				B1 = 30 * log10(4 * PI) + SystemLoss;

				double C1, C2, C3, C4;
				if (m_Switch.RD_HasClutter)
					C2 = 0;//pow(10.0, m_Clutter.CPower(NEDRadarState, NEDTargetState, NUERadarState, NUETargetState) / 10.0);					//��/���Ӳ����Ź��ʡ���
				else
					C2 = 0;
				C3 = pow(10.0, m_Receiver.CalNPower() / 10.0);					//�״�ϵͳ���������ʡ���
				if (m_Switch.RD_HasJam)
					C4 = CJam; //���Ż�����;��λ����
				else
					C4 = 0;
				if (ENUTargetState.tPos.z > ENURadarState.rPos.z)   //�����أ�zΪ��ֱ����
					C1 = 10.0 * log10(C2 + C3 + C4);        //ȫ���������ʡ�DB
				else
					C1 = m_Receiver.CalNPower();

				double DetectRange;
				DetectRange = pow(10, (A1 - B1 - C1 - 10 * log10(SNRThreshold)) /
					40);	//�ܹ�̽�����

  //�����ۻ�������ΪR���������ΪSNR2,���ָ���ΪPd2
				double SNR2 = pow(10, (A1 - B1 - C1 - 10 * log10(pow(range, 4))) / 10);
				double bSNR2 = (pow(10, SNR2 / 10.)) / 1.7;
				double Pd2 = 0.0;
				if (bSNR2 >= 20)
					Pd2 = 1;
				else
					Pd2 = exp(bSNR2) / (1. + exp(bSNR2));

				//std::cout << Pd2 << std::endl;

				//�����뿼�Ǹ��ʣ�ֱ������Pd2 = 1;
				srand(time(NULL));
				double randnum = ((double)rand()) / ((double)RAND_MAX);
				if (randnum <= Pd2 && range <= DetectRange)
					ENUTargetState.detected = true;
				else
					ENUTargetState.detected = false;
			}
		}
	}


	void Radar::Set_JamPower(double JamP)
	{
		CJam = JamP;
	}

	void Radar::Set_radar_state(Vector3& Pos, Vector3& Vel) {
		ENURadarState.rVel = Vel;
		ENURadarState.rPos = Pos;
		//ENURadarState.fre_center = 9.8e9;
		//ENURadarState.fre_band = 0.2e9;
	}

	string Radar::Get_detect_result()
	{
		if (ENUTargetState.detected)
		{
			return "Detected Success";
		}
		else
		{
			return "Detected Failure";
		}
	}

}
