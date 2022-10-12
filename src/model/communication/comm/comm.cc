#define BOUNDS_CHECK
#include "comm.h"
#include <iostream>
#include <iomanip> 
#include <inverse.h>  

using namespace splab;

bool Comm::Init(
  const std::unordered_map<std::string, std::any> &value
) {
  if(auto it = value.find("rec_para"); it != value.end()) {
	rec_para_.FromValueMap(std::any_cast<CSValueMap>(it->second));
  }
  if(auto it = value.find("tran_para"); it != value.end()) {
	tran_para_.FromValueMap(std::any_cast<CSValueMap>(it->second));
  }
  if (auto it = value.find("environment"); it != value.end()) {
	  environment_.FromValueMap(std::any_cast<CSValueMap>(it->second));
  }

  state_ = CSInstanceState::IS_INITIALIZED;
  WriteLog("Comm model Init", 1);
  return true;
}

bool Comm::Tick(double time) {
  params_.clear();
  
  // 此处填写模型单步运算逻辑
  // 需输出的参数应通过emplace方法写入params_
  // 等候GetOutput接口被调用时参数对外部输出
  tran_Gain();
  rec_Gain();
  N_t();
  N();
  postion();
  GAIN();
  Ber();
  Get_tran();
  params_.emplace("Get_trangtf", Get_trangtf());
  params_.emplace("BER", ber);
  std::cout << std::scientific << ber << std::endl;
  params_.emplace("Get_trangt", Get_trangt());
  params_.emplace("Get_trangtB", Get_trangB());
  params_.emplace("Get_tranp", Get_tranp());

  params_.emplace("connect", connect);
  std::cout << connect << std::endl;
  params_.emplace("Get_tranpos", Get_tranpos());
  
  WriteLog("Comm model Tick", 1);
  return true;
}

bool Comm::SetInput(
  const std::unordered_map<std::string, std::any> &value
) {
  if(auto it = value.find("position"); it != value.end()) {
	position_.FromValueMap(std::any_cast<CSValueMap>(it->second));
  }
  if(auto it = value.find("vehicle"); it != value.end()) {
	vehicle_.FromValueMap(std::any_cast<CSValueMap>(it->second));
  }
 
  if(auto it = value.find("tran"); it != value.end()) {
	tran_.FromValueMap(std::any_cast<CSValueMap>(it->second));
  }
 
  if (auto it = value.find("j"); it != value.end()) {
	  j_ = std::any_cast<double>(it->second);
  }

  WriteLog("Comm model SetInput", 1);
  return true;
}



std::unordered_map<std::string, std::any>* Comm::GetOutput() {
  state_ = CSInstanceState::IS_RUNNING;
  params_.emplace("ForceSideID", GetForceSideID());
  params_.emplace("ModelID", GetModelID());
  params_.emplace("InstanceName", GetInstanceName());
  params_.emplace("ID", GetID());
  params_.emplace("State", uint16_t(GetState()));
  params_.emplace("out", Get_tran());
  WriteLog("Comm model GetOutPut", 1);
  return &params_;
}

std::unordered_map<std::string, std::any> Comm::Get_tranpos()//得到设备位置
{
	return this->position_.ToValueMap();
}

double Comm::Get_tranp()//得到设备发射功率
{
	return std::any_cast<double>(this->tran_para_.ToValueMap()["Comm_Set_Power"]);
}

double Comm::Get_trangt()//得到设备发射天线最大增益
{
	return Gt;
}

double Comm::Get_trangtf()//得到设备发射频率
{
	return std::any_cast<double>(this->tran_para_.ToValueMap()["Comm_Set_Freq"]);
}

double Comm::Get_trangB()//得到通信带宽
{
	return std::any_cast<double>(this->tran_para_.ToValueMap()["Comm_Set_Freq_Bw"]);
}

std::unordered_map<std::string, std::any> Comm::Get_tran()
{
	std::unordered_map<std::string, std::any> postion2 =
	{
		 {"x", std::any_cast<double>(position_.ToValueMap()["x"])},
		 {"y", std::any_cast<double>(position_.ToValueMap()["y"])},
		 {"z", std::any_cast<double>(position_.ToValueMap()["z"])}
			 };
	std::unordered_map<std::string, std::any> tran = {
		 {"tran_pos", postion2},
		 {"tran_p", Get_tranp()},
		 {"tran_gt", Get_trangt()},
		 {"tran_tf", Get_trangtf()},
	   {"tran_B", Get_trangB()}
	};
	return tran;
}

double Comm::Gain(double f, int i, double A)//天线增益
{
	double pi = 3.14159265359;
	double C = 3e08;
	double e;
	double Gm;
	double lambda = C / (f * 1e09);
	switch (i) {
	case 1://喇叭天线
		e = 0.81;
		Gm = 4 * pi / (lambda * lambda) * A * e;
		break;
	case 2://抛物面或碟盘天线
		e = 0.56;
		Gm = 4 * pi / (lambda * lambda) * A * e;
		break;
	case 3://绕杆式天线
		Gm = 1.15;
		break;
	case 4://半波极子
		Gm = 1.64;
		break;
	}
	//G = 10 * log10(Gm);
	return Gm;
}

void Comm::tran_Gain()//发射天线增益
{
	Gt = Gain(std::any_cast<double>(tran_para_.ToValueMap()["Comm_Set_Freq"]), std::any_cast<int32_t>(tran_para_.ToValueMap()["Comm_Set_i"]), std::any_cast<double>(tran_para_.ToValueMap()["Comm_Set_A"]));
}

void Comm::rec_Gain()//接收天线增益
{
	Gr = Gain(std::any_cast<double>(rec_para_.ToValueMap()["Comm_Set_Freq"]), std::any_cast<int32_t>(rec_para_.ToValueMap()["Comm_Set_i"]), std::any_cast<double>(rec_para_.ToValueMap()["Comm_Set_A"]));
}

void Comm::N_t()//环境噪声计算
{
	double N_R, N_C, N_A;
	switch (std::any_cast<int32_t>(environment_.ToValueMap()["Rain"])) {
	case 1:
		N_R = 0;
		break;
	case 2:
		N_R = 0;
		break;
	case 3:
		N_R = 0;
		break;
	}
	switch (std::any_cast<int32_t>(environment_.ToValueMap()["Cloud"])) {
	case 1:
		N_C = 0;
		break;
	case 2:
		N_C = 0;
		break;
	case 3:
		N_C = 0;
		break;
	}
	switch (std::any_cast<int32_t>(environment_.ToValueMap()["Atmosphere"])) {
	case 1:
		N_A = 0;
		break;
	case 2:
		N_A = 0;
		break;
	case 3:
		N_A = 0;
		break;
	}
	Nt = N_R + N_C + N_A;
}

double Comm::Atmos()//大气吸收模型
{
	Atmosphere A;
	double T = 288.16 - 0.0065 * std::any_cast<double>(position_.ToValueMap()["z"]) / (1 + std::any_cast<double>(position_.ToValueMap()["z"]) / 6371e03);
	double P = 1013.25 * pow(T / 288.16, 5.25612220);
	//氧气吸收因子
	A.oxygen = 0.0;
	double AN = 0.0;//谐振数求和项
	int N;//谐振数
	double FN_p, FN_n, F0;
	double uN_p, uN_n, uN0;
	double eN;
	//氧气的谐振频率
	std::unordered_map<int, std::pair<double, double>> fN = { {1,{56.2648,118.7507}},{3,{58.4466,62.4863}}
	,{5,{59.5910,60.3601}},{7,{60.4348,59.1642}},{9,{61.1506,58.3239}},{11,{61.8002,57.6125}}
	,{13,{62.4112,56.9682}},{15,{62.9980,56.3634}},{17,{63.5685,55.7839}},{19,{64.1272,55.2214}}
	,{21,{64.6779,54.6728}},{23,{65.2204,54.1294}} };
	//电磁波频率
	double f = std::any_cast<double>(rec_para_.ToValueMap()["Comm_Set_Freq"]);
	double gh;
	if (std::any_cast<double>(position_.ToValueMap()["z"]) <= 8)gh = 0.640;
	else gh = 0.3026 + 0.04218 * std::any_cast<double>(position_.ToValueMap()["z"]);
	double deltaf = 0.1895 * gh * P / T;
	F0 = deltaf / (f * f + deltaf * deltaf);
	for (N = 1; N <= 23; N = N + 2)
	{
		double n = double(N);
		eN = 2.06844 * n * (n + 1.0);
		uN_p = n * (2 * n + 3) / (n + 1), uN_n = (n + 1) * (2 * n - 1) / n, uN0 = 2 * (n * n + n + 1) * (2 * n + 1) / (n * (n + 1));
		FN_p = fN[N].first, FN_n = fN[N].second;
		AN = AN + (FN_p * uN_p + FN_n * uN_n + F0 * uN0) * exp(-eN / T);
	}
	A.oxygen = 2.0058 * P * pow(T, -3.0) * f * f * AN;
	//水蒸气吸收因子模型
	A.steam = 0.0;
	std::vector<double> c = { 5.9685,-2.0163,0.2622,-0.0163,0.0005,-0.00000528 };
	double rho = 0.0;
	for (int i = 0; i < 6; i++)
	{
		rho = rho + c[i] * pow(std::any_cast<double>(position_.ToValueMap()["z"]), double(i));
	}
	double cathy_22, cathy_res;
	double Pw = rho * T / 288.75;
	double deltaf2 = 0.01799 * (Pw * 300.0 / T + 0.20846 * (0.75 * P - Pw) * pow((300.0 / T), 0.63));
	double F = f / 100.0 * (deltaf2 / ((100.0 - f) * (100.0 - f) + deltaf2 * deltaf2) + deltaf2 / ((100.0 + f) * (100.0 + f) + deltaf2 * deltaf2));
	cathy_22 = 0.002535 * (f * Pw * pow(300.0 / T, 3.5) * exp(2.144 * (1 - 300.0 / T)) * F);
	cathy_res = 0.007347 * rho * P * pow(T, -2.5) * f * f;
	A.steam = cathy_22 + cathy_res;
	double N_Atmos = A.oxygen + A.steam;
	return N_Atmos;
}

void Comm::N()
{
	NJ = 10 * log10(j_) + 228.6 - 10 * log10(std::any_cast<double>(rec_para_.ToValueMap()["Comm_Set_Freq_Bw"]));
	N0 = NJ + Nt;
}

void Comm::postion()
{
	V3f tran_pos = std::any_cast<V3f>(tran_.ToValueMap()["tran_pos"]);
	//tran_pos.FromValueMap(std::any_cast<CSValueMap>(tran_.ToValueMap()["tran_pos"]));
	double deltax = std::any_cast<double>(position_.ToValueMap()["x"]) - std::any_cast<double>(tran_pos.ToValueMap()["x"]);
	double deltay = std::any_cast<double>(position_.ToValueMap()["y"]) - std::any_cast<double>(tran_pos.ToValueMap()["y"]);
	double deltaz = std::any_cast<double>(position_.ToValueMap()["z"]) - std::any_cast<double>(tran_pos.ToValueMap()["z"]);
	rel.azimuth = atan(deltay / deltax) / 3.1415926 * 180.0;
	rel.altitude = atan(deltaz / pow((deltax * deltax + deltay * deltay), 0.5)) / 3.1415926 * 180.0;
	rel.d = pow((deltax * deltax + deltay * deltay + deltaz * deltaz), 0.5);
}

void Comm::GAIN()
{
	double Gm = Gr * std::any_cast<double>(tran_.ToValueMap()["tran_gt"]);
	//利用SP++库进行坐标转换，将机体坐标系转换到地面坐标系
	double xb = 1.0, yb = tan(std::any_cast<double>(rec_para_.ToValueMap()["Comm_Set_azi"]) / 180 * 3.1415926), zb = tan(std::any_cast<double>(rec_para_.ToValueMap()["Comm_Set_alt"]) / 180 * 3.1415926) * pow((1 + tan(std::any_cast<double>(rec_para_.ToValueMap()["Comm_Set_azi"]) / 180 * 3.1415926) * tan(std::any_cast<double>(rec_para_.ToValueMap()["Comm_Set_azi"]) / 180 * 3.1415926)), 0.5);
	Matrix<double> PB(3, 1);
	PB[0][0] = xb, PB[1][0] = yb, PB[2][0] = zb;
	Matrix<double> S_thelta_psi_phi(3, 3);
	S_thelta_psi_phi[0][0] = cos(std::any_cast<double>(vehicle_.ToValueMap()["y"])) * cos(std::any_cast<double>(vehicle_.ToValueMap()["x"])), S_thelta_psi_phi[0][1] = cos(std::any_cast<double>(vehicle_.ToValueMap()["y"])) * sin(std::any_cast<double>(vehicle_.ToValueMap()["x"])), S_thelta_psi_phi[0][2] = -sin(std::any_cast<double>(vehicle_.ToValueMap()["y"])),
		S_thelta_psi_phi[1][0] = sin(std::any_cast<double>(vehicle_.ToValueMap()["y"])) * cos(std::any_cast<double>(vehicle_.ToValueMap()["x"])) * sin(std::any_cast<double>(vehicle_.ToValueMap()["z"])) - sin(std::any_cast<double>(vehicle_.ToValueMap()["x"])) * cos(std::any_cast<double>(vehicle_.ToValueMap()["z"])), S_thelta_psi_phi[1][1] = sin(std::any_cast<double>(vehicle_.ToValueMap()["y"])) * sin(std::any_cast<double>(vehicle_.ToValueMap()["x"])) * sin(std::any_cast<double>(vehicle_.ToValueMap()["z"])) + cos(std::any_cast<double>(vehicle_.ToValueMap()["x"])) * cos(std::any_cast<double>(vehicle_.ToValueMap()["z"])), S_thelta_psi_phi[1][2] = cos(std::any_cast<double>(vehicle_.ToValueMap()["y"])) * sin(std::any_cast<double>(vehicle_.ToValueMap()["z"])),
		S_thelta_psi_phi[2][0] = sin(std::any_cast<double>(vehicle_.ToValueMap()["y"])) * cos(std::any_cast<double>(vehicle_.ToValueMap()["x"])) * sin(std::any_cast<double>(vehicle_.ToValueMap()["z"])) + sin(std::any_cast<double>(vehicle_.ToValueMap()["x"])) * cos(std::any_cast<double>(vehicle_.ToValueMap()["z"])), S_thelta_psi_phi[2][1] = sin(std::any_cast<double>(vehicle_.ToValueMap()["y"])) * sin(std::any_cast<double>(vehicle_.ToValueMap()["x"])) * sin(std::any_cast<double>(vehicle_.ToValueMap()["z"])) - cos(std::any_cast<double>(vehicle_.ToValueMap()["x"])) * cos(std::any_cast<double>(vehicle_.ToValueMap()["z"])), S_thelta_psi_phi[2][2] = cos(std::any_cast<double>(vehicle_.ToValueMap()["y"])) * cos(std::any_cast<double>(vehicle_.ToValueMap()["z"]));
	Matrix<double> S = trT(S_thelta_psi_phi);
	Matrix<double> PG = S * PB;
	double xg = PG[0][0], yg = PG[1][0], zg = PG[2][0];
	azig = atan(yg / xg) / 3.1415926 * 180.0, altg = atan(zg / pow((xg * xg + yg * yg), 0.5)) / 3.1415926 * 180.0;
	if (abs(altg) <= 30.0)
		G = Gm;
	else
		//G = Gm - (Gm / 60.0) * abs(abs(rel.altitude - altg) - 30.0);
		G = Gm / 1000.0;
	if (abs(azig) >= 60.0)
		G = G / (abs(azig) - 60.0);
	G = 10 * log10(G);
}

void Comm::Ber()
{
	double Lf = 92.44 + 20 * log10(rel.d) + 20 * log10(std::any_cast<double>(tran_para_.ToValueMap()["Comm_Set_Freq"]));//自由空间损耗
	double B = std::any_cast<double>(tran_para_.ToValueMap()["Comm_Set_Freq_Bw"]);
	double P_n = -228.6 + 10 * log10(std::any_cast<double>(rec_para_.ToValueMap()["Comm_Set_T"])) + 10 * log10(B) + N0;
	double P_t = std::any_cast<double>(tran_.ToValueMap()["tran_p"]);
	double P_r = 10 * log10(P_t) + G - Lf;
	double CN = P_r - P_n - Atmos() * rel.d / 1e03;
	double EbN0 = pow(10, CN / 20);
	ber = 0.5 * erfc(EbN0);
	if (ber > 1e-05)
		connect = false;
	else
		connect = true;
}

COMM_EXPORT CSModelObject* CreateModelObject() {
  CSModelObject* model = new Comm();
  return model;
}

COMM_EXPORT void DestroyMemory(void* mem, bool is_array) {
  if(is_array) {
    delete[]((Comm*)mem);
  } else {
    delete ((Comm*)mem);
  }
}
