
#define BOUNDS_CHECK 

#include "Comm.h"
#include <string>
#include <vector>
#include <iomanip> 
#include "SP++3.0/matrix.h"

namespace externModel::comm{

using namespace std;
using namespace splab;

Comm::Comm()
{

}
Comm::~Comm(){}

bool Comm::Comm_test(double Posa_x, double Posa_y, double Posa_z,
	double Posb_x, double Posb_y, double Posb_z, double FT_comm)
{
	static Comm comm1, comm2;
	Comm* c1 = &comm1;
	Comm* c2 = &comm2;
	comm1.Init();
	comm2.Init();
	comm1.Comm_Init(c2);
	comm2.Comm_Init(c1);
	//Setinput
	Env environment;
	environment.Atmosphere = 1;
	environment.Cloud = 1;
	environment.Rain = 1;
	facilityPOS pos1;
	pos1.Comm_Set_longitude = Posa_x;
	pos1.Comm_Set_latitude = Posa_y;
	pos1.Comm_Set_altitude = Posa_z;
	double jam = 1e-16;
	carrier plat1;
	plat1.psi = 0.0;
	plat1.thelta = 0.0;
	plat1.phi = 0.0;
	//一号设备
	comm1.SetInput(pos1, environment, jam, plat1);
	facilityPOS pos2;
	pos2.Comm_Set_longitude = Posb_x;
	pos2.Comm_Set_latitude = Posb_y;
	pos2.Comm_Set_altitude = Posb_z;
	carrier plat2;
	plat2.psi = 0;
	plat2.thelta = 0;
	plat2.phi = 0;
	//二号设备
	comm2.SetInput(pos2, environment, jam, plat2);
	comm1.Tick(FT_comm*1000);
	comm2.Tick(FT_comm*1000);
	comm2.Get();
	double ber = comm2.Getber();
	//cout << '\t' << ber << '\n' << endl;
	bool connect = comm2.Getconnect();
	return connect;
}

bool Comm::Init(){
	tran_para.Comm_Set_A = 10.0;
	tran_para.Comm_Set_i = 1;
	tran_para.Comm_Set_azi = 0.0;
	tran_para.Comm_Set_alt = 0.0;
	tran_para.Comm_Set_Freq = 3.5e-01;
	tran_para.Comm_Set_Freq_Bw = 36e06;
	tran_para.Comm_Set_Power = 100.0;
	tran_para.Comm_Set_T = 500.0;
	rec_para.Comm_Set_A = 0.5;
	rec_para.Comm_Set_i = 2;
	rec_para.Comm_Set_azi = 0.0;
	rec_para.Comm_Set_alt = 0.0;
	rec_para.Comm_Set_Freq = 3.5e-01;
	rec_para.Comm_Set_Freq_Bw = 36e06;
	rec_para.Comm_Set_Power = 100.0;
	rec_para.Comm_Set_T = 500.0;
	
	return true;
}

bool Comm::tran_Init(const Fixed_para& tran)
{
	tran_para.Comm_Set_A = tran.Comm_Set_A;
	tran_para.Comm_Set_i = tran.Comm_Set_i;
	tran_para.Comm_Set_azi = tran.Comm_Set_azi;
	tran_para.Comm_Set_alt = tran.Comm_Set_alt;
	tran_para.Comm_Set_Freq = tran.Comm_Set_Freq;
	tran_para.Comm_Set_Freq_Bw = tran.Comm_Set_Freq_Bw;
	tran_para.Comm_Set_Power = tran.Comm_Set_Power;
	tran_para.Comm_Set_T = tran.Comm_Set_T;
	return true;
}

bool Comm::rec_Init(const Fixed_para& rec)
{
	rec_para.Comm_Set_A = rec.Comm_Set_A;
	rec_para.Comm_Set_i = rec.Comm_Set_i;
	rec_para.Comm_Set_azi = rec.Comm_Set_azi;
	rec_para.Comm_Set_alt = rec.Comm_Set_alt;
	rec_para.Comm_Set_Freq = rec.Comm_Set_Freq;
	rec_para.Comm_Set_Freq_Bw = rec.Comm_Set_Freq_Bw;
	rec_para.Comm_Set_Power = rec.Comm_Set_Power;
	rec_para.Comm_Set_T = rec.Comm_Set_T;
	return true;
}

bool Comm::Comm_Init(Comm* comm1)
{
	Comm_tran = comm1;
	return true;
}

bool Comm::Tick(double time) 
{
	tran_Gain();
	rec_Gain();
	N_t();
	N();
	postion();
	GAIN();
	//BER();
	Ber();
	return true;
}

bool Comm::SetInput(const facilityPOS& postion, const Env& environment, const double& j, const carrier& vehicle)
{
	comm_pos.Comm_Set_longitude = postion.Comm_Set_longitude;
	comm_pos.Comm_Set_latitude = postion.Comm_Set_latitude;
	comm_pos.Comm_Set_altitude = postion.Comm_Set_altitude;
	env.Rain = environment.Rain;
	env.Atmosphere = environment.Atmosphere;
	env.Cloud = environment.Cloud;
	NJ = 10 * log10(j) + 228.6 - 10 * log10(rec_para.Comm_Set_Freq_Bw);
	platform.psi = vehicle.psi;
	platform.thelta = vehicle.thelta;
	platform.phi = vehicle.phi;
	return true;
}

facilityPOS Comm::Get_tranpos()
{
	return this->comm_pos;
}

double Comm::Get_tranp()
{
	return this->tran_para.Comm_Set_Power;
}

double Comm::Get_trangt()
{
	return this->Gt;
}

double Comm::Get_trangtf()
{
	return this->tran_para.Comm_Set_Freq;
}

double Comm::Get_trangB()
{
	return this->tran_para.Comm_Set_Freq_Bw;
}

double Comm::Gain(double f,int i,double A)
{
	double pi = 3.14159265359;
	double C = 3e08;
	double e;
	double Gm;
	double lambda = C / (f * 1e09);
	Gm = 0;
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

void Comm::tran_Gain()
{
	Gt = Gain(tran_para.Comm_Set_Freq, tran_para.Comm_Set_i, tran_para.Comm_Set_A);
}

void Comm::rec_Gain()
{
	Gr = Gain(rec_para.Comm_Set_Freq, rec_para.Comm_Set_i, rec_para.Comm_Set_A);
}

void Comm::N_t()//暂时无视
{
	switch (env.Rain) {
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
	switch (env.Cloud) {
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
	switch (env.Atmosphere) {
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

double Comm::Atmos()
{
	Atmosphere A;
	double T = 288.16 - 0.0065 * (comm_pos.Comm_Set_altitude / 1e03) / (1 + (comm_pos.Comm_Set_altitude / 1e03) / 6371e03);
	double P = 1013.25 * pow(T / 288.16, 5.25612220);
	//氧气吸收因子
	A.oxygen = 0.0;
	double AN = 0.0;//谐振数求和项
	int N;//谐振数
	double FN_p, FN_n, F0;
	double uN_p, uN_n, uN0;
	double eN;
	//氧气的谐振频率
	unordered_map<int, pair<double, double>> fN = { {1,{56.2648,118.7507}},{3,{58.4466,62.4863}}
	,{5,{59.5910,60.3601}},{7,{60.4348,59.1642}},{9,{61.1506,58.3239}},{11,{61.8002,57.6125}}
	,{13,{62.4112,56.9682}},{15,{62.9980,56.3634}},{17,{63.5685,55.7839}},{19,{64.1272,55.2214}}
	,{21,{64.6779,54.6728}},{23,{65.2204,54.1294}} };
	//电磁波频率
	double f = rec_para.Comm_Set_Freq;
	double gh;
	if (comm_pos.Comm_Set_altitude <= 8)gh = 0.640;
	else gh = 0.3026 + 0.04218 * comm_pos.Comm_Set_altitude;
	double deltaf = 0.1895 * gh * P / T;
	F0 = deltaf / (f * f + deltaf * deltaf);
	for (N = 1; N <= 23; N = N + 2)
	{
		double n = static_cast<double>(N);
		eN = 2.06844 * n * (n + 1.0);
		uN_p = n * (2 * n + 3) / (n + 1), uN_n = (n + 1) * (2 * n - 1) / n, uN0 = 2 * (n * n + n + 1) * (2 * n + 1) / (n * (n + 1));
		FN_p = fN[N].first, FN_n = fN[N].second;
		AN = AN + (FN_p * uN_p + FN_n * uN_n + F0 * uN0) * exp(-eN / T);
	}
	A.oxygen = 2.0058 * P * pow(T, -3.0) * f * f * AN;
	//水蒸气吸收因子模型
	A.steam = 0.0;
	vector<double> c = { 5.9685,-2.0163,0.2622,-0.0163,0.0005,-0.00000528 };
	double rho = 0.0;
	for (int i = 0; i < 6; i++)
	{
		rho = rho + c[i] * pow(comm_pos.Comm_Set_altitude, static_cast<double>(i));
	}
	double cathy_22, cathy_res;
	double Pw = rho * T / 288.75;
	double deltaf2 = 0.01799 * (Pw * 300.0 / T + 0.20846 * (0.75 * P - Pw) * pow((300.0 / T), 0.63));
	double F = f / 100.0 * (deltaf2 / ((100.0 - f) * (100.0 - f) + deltaf2 * deltaf2) + deltaf2 / ((100.0 + f) * (100.0 + f) + deltaf2 * deltaf2));
	cathy_22 = 0.002535 * (f * Pw * pow(300.0 / T, 3.5) * exp(2.144 * (1 - 300.0 / T)) * F);
	cathy_res = 0.007347 * rho * P * pow(T, -2.5) * f * f;
	double N_Atmos = A.oxygen + A.steam;
	return N_Atmos;
}

double Comm::refraction()
{
	//大气透镜效应损耗
	double Len = 0.0;
	double R = rel.d;
	vector<double> c = { -3.633e-3,2.847e-4,3.795e-6,-3.390e-9 };
	for (int i = 0; i < 4; i++)
	{
		Len = Len + c[i] * pow(R, i);
	}
	//大气折射效应（暂时无视）
	return 0;
}

void Comm::N()
{
	N0 = NJ + Nt;
}

void Comm::postion()
{
	tran_pos = Comm_tran->Get_tranpos();
	double deltax = comm_pos.Comm_Set_longitude - tran_pos.Comm_Set_longitude;
	double deltay = comm_pos.Comm_Set_latitude - tran_pos.Comm_Set_latitude;
	double deltaz = comm_pos.Comm_Set_altitude - tran_pos.Comm_Set_altitude;
	rel.azimuth = atan2(deltay, deltax)/3.1415926*180.0;
	rel.altitude = atan2(deltaz, pow((deltax * deltax + deltay * deltay), 0.5)) / 3.1415926 * 180.0;
	rel.d = pow((deltax * deltax + deltay * deltay + deltaz * deltaz), 0.5);
}

 void Comm::GAIN()
 {
 	double Gm = Gr * Comm_tran->Get_trangt();
 	//利用SP++库进行坐标转换，将机体坐标系转换到地面坐标系
 	double xb = 1.0, yb = tan(rec_para.Comm_Set_azi / 180 * 3.1415926), zb = tan(rec_para.Comm_Set_alt / 180 * 3.1415926) * pow((1 + tan(rec_para.Comm_Set_azi / 180 * 3.1415926) * tan(rec_para.Comm_Set_azi / 180 * 3.1415926)), 0.5);
	Matrix<double> PB(3,1);
	PB[0][0] = xb, PB[1][0] = yb, PB[2][0] = zb;
	Matrix<double> S_thelta_psi_phi(3, 3);
	S_thelta_psi_phi[0][0] = cos(platform.thelta) * cos(platform.psi), S_thelta_psi_phi[0][1] = cos(platform.thelta) * sin(platform.psi), S_thelta_psi_phi[0][2] = -sin(platform.thelta),
	S_thelta_psi_phi[1][0] = sin(platform.thelta) * cos(platform.psi) * sin(platform.phi) - sin(platform.psi) * cos(platform.phi), S_thelta_psi_phi[1][1] = sin(platform.thelta) * sin(platform.psi) * sin(platform.phi) + cos(platform.psi) * cos(platform.phi), S_thelta_psi_phi[1][2] = cos(platform.thelta) * sin(platform.phi),
	S_thelta_psi_phi[2][0] = sin(platform.thelta) * cos(platform.psi) * sin(platform.phi) + sin(platform.psi) * cos(platform.phi), S_thelta_psi_phi[2][1] = sin(platform.thelta) * sin(platform.psi) * sin(platform.phi) - cos(platform.psi) * cos(platform.phi), S_thelta_psi_phi[2][2] = cos(platform.thelta) * cos(platform.phi);
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

std::unordered_map<double, bool> Comm::BER()//计算误码率并将计算结果保存在map里
{
	double Lf = 92.44 + 20 * log10(rel.d) + 20 * log10(tran_para.Comm_Set_Freq);//自由空间损耗
	double B = tran_para.Comm_Set_Freq_Bw;
	double P_n = -228.6 + 10 * log10(rec_para.Comm_Set_T) + 10 * log10(B) + N0;
	double P_t= Comm_tran->Get_tranp();
	double P_r = 10 * log10(P_t) + G - Lf;
	double CN = P_r - P_n - Atmos() * rel.d / 1e03;
	double EbN0 = pow(10, CN / 20);
	ber = 0.5 * erfc(EbN0);
	if (ber > 1e-05)
		connect = false;
	else
		connect = true;
	result.insert(std::pair<double, bool>(ber, connect));
	return result;
}

void Comm::Ber() //计算误码率
{
	double Lf = 92.44 + 20 * log10(rel.d) + 20 * log10(tran_para.Comm_Set_Freq);//自由空间损耗
	double B = tran_para.Comm_Set_Freq_Bw;
	double P_n = -228.6 + 10 * log10(rec_para.Comm_Set_T) + 10 * log10(B) + N0;
	double P_t = Comm_tran->Get_tranp();
	double P_r = 10 * log10(P_t) + G - Lf;
	double CN = P_r - P_n - Atmos() * rel.d / 1e03;
	double EbN0 = pow(10, CN / 20);
	ber = 0.5 * erfc(EbN0);
	if (ber > 1e-05)
		connect = false;
	else
		connect = true;
}

std::unordered_map<double, bool> Comm::GetBER()
{
	return result;
}

void Comm::Get()
{
	//std::cout << std::scientific << rel.azimuth << " " << rel.altitude << " " << rel.d << std::endl;
	//std::cout << std::scientific << azig << " " << altg << " " << rel.d << std::endl;
	//std::cout << std::scientific << ber << " " << connect << std::endl;
	//cout << rel.azimuth << " " << rel.altitude << " " << rel.d << endl;
	////cout << azig << " " << altg << " " << rel.d << endl;
	//cout << "BER:" << ber << " " << " CONNET:" << connect << endl;
	//cout << "\t" << endl;
}

double Comm::Getber()
{
	return ber;
}
bool Comm::Getconnect()
{
	return connect;
}

}