#pragma once
#include <string>
#include <sstream>  

namespace externModel::comm{

using namespace std;

template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}
struct Fixed_para {
	double Comm_Set_A;//���ߵĿ������
	int Comm_Set_i;//��������
	double Comm_Set_azi;//����ָ��ķ����
	double Comm_Set_alt;//����ָ��ĸ߶Ƚ�
	double Comm_Set_Freq_Bw;//����
	double Comm_Set_Freq;//�豸Ƶ��GHz
	double Comm_Set_T;//ϵͳ�����¶�
	double Comm_Set_Power;//���书��

};//Commģ�͵��豸��Ϣ

struct Comm_Simple_para {
	//double Comm_Set_A;//���ߵĿ������
	//int Comm_Set_i;//��������
	//double Comm_Set_azi;//����ָ��ķ����
	//double Comm_Set_alt;//����ָ��ĸ߶Ƚ�
	double Comm_Set_G;//��������
	double Comm_Set_Freq_Bw;//����
	double Comm_Set_Freq;//�豸Ƶ��GHz
	double Comm_Set_T;//ϵͳ�����¶�
	double Comm_Set_Power;//���书��

};//Comm_Simpleģ�͵��豸��Ϣ

struct Comm_Complex_para {
	//double Comm_Set_A;//���ߵĿ������
	//int Comm_Set_i;//��������
	double Comm_Set_azi;//����ָ��ķ����
	double Comm_Set_alt;//����ָ��ĸ߶Ƚ�
	double Comm_Set_Freq_Bw;//����
	double Comm_Set_Freq;//�豸Ƶ��GHz
	double Comm_Set_T;//ϵͳ�����¶�
	double Comm_Set_Power;//���书��

};//Comm_Complexģ�͵��豸��Ϣ

struct facilityPOS {
	double Comm_Set_longitude;//�豸�ľ���
	double Comm_Set_latitude;//�豸��γ��
	double Comm_Set_altitude;//�豸�ĸ߶�
};

struct carrier {//������̬��Ϣ
	double psi;//ƫ����
	double thelta;//������
	double phi;//��ת��
};

struct Env {
	int Rain;//����ȼ�
	int Atmosphere;//����������ĵȼ�
	int Cloud;//�����ȼ�
};

struct Atmosphere {
	double oxygen;
	double steam;
};

struct relative_POS {
	double azimuth;
	double altitude;
	double d;//km

};//���ջ�����ڷ���������λ��

//struct antenna_para {
//	string type;
//};

}