#include "model_struct.h"

// 本文件记录模型所使用的复杂数据结构

// V3f V3f
class V3f{
 public:
  virtual CSValueMap ToValueMap(){
	CSValueMap value;
    value.emplace("x", x);
    value.emplace("y", y);
    value.emplace("z", z);
	
	return std::move(value);
  }
  
  virtual void FromValueMap(CSValueMap &value){
	for(auto &it : value) {
	  if(it.first == "x")
		x = std::any_cast<double>(value["x"]);
	  if(it.first == "y")
		y = std::any_cast<double>(value["y"]);
	  if(it.first == "z")
		z = std::any_cast<double>(value["z"]);
	}
  }
  
 private:
  //x x
  double x; 
  //y y
  double y; 
  //z z
  double z; 
};

// Env Env
class Env{
 public:
  virtual CSValueMap ToValueMap(){
	CSValueMap value;
    value.emplace("Rain", Rain);
    value.emplace("Atmosphere", Atmosphere);
    value.emplace("Cloud", Cloud);
	
	return std::move(value);
  }
  
  virtual void FromValueMap(CSValueMap &value){
	for(auto &it : value) {
	  if(it.first == "Rain")
		Rain = std::any_cast<int32_t>(value["Rain"]);
	  if(it.first == "Atmosphere")
		Atmosphere = std::any_cast<int32_t>(value["Atmosphere"]);
	  if(it.first == "Cloud")
		Cloud = std::any_cast<int32_t>(value["Cloud"]);
	}
  }
  
 private:
  //Rain Rain
  int32_t Rain; 
  //Atmosphere Atmosphere
  int32_t Atmosphere; 
  //Cloud Cloud
  int32_t Cloud; 
};

// Fixed_para Fixed_para
class Fixed_para{
 public:
  virtual CSValueMap ToValueMap(){
	CSValueMap value;
    value.emplace("Comm_Set_A", Comm_Set_A);
    value.emplace("Comm_Set_i", Comm_Set_i);
    value.emplace("Comm_Set_azi", Comm_Set_azi);
    value.emplace("Comm_Set_alt", Comm_Set_alt);
    value.emplace("Comm_Set_Freq_Bw", Comm_Set_Freq_Bw);
    value.emplace("Comm_Set_Freq", Comm_Set_Freq);
    value.emplace("Comm_Set_T", Comm_Set_T);
    value.emplace("Comm_Set_Power", Comm_Set_Power);
	
	return std::move(value);
  }
  
  virtual void FromValueMap(CSValueMap &value){
	for(auto &it : value) {
	  if(it.first == "Comm_Set_A")
		Comm_Set_A = std::any_cast<double>(value["Comm_Set_A"]);
	  if(it.first == "Comm_Set_i")
		Comm_Set_i = std::any_cast<int32_t>(value["Comm_Set_i"]);
	  if(it.first == "Comm_Set_azi")
		Comm_Set_azi = std::any_cast<double>(value["Comm_Set_azi"]);
	  if(it.first == "Comm_Set_alt")
		Comm_Set_alt = std::any_cast<double>(value["Comm_Set_alt"]);
	  if(it.first == "Comm_Set_Freq_Bw")
		Comm_Set_Freq_Bw = std::any_cast<double>(value["Comm_Set_Freq_Bw"]);
	  if(it.first == "Comm_Set_Freq")
		Comm_Set_Freq = std::any_cast<double>(value["Comm_Set_Freq"]);
	  if(it.first == "Comm_Set_T")
		Comm_Set_T = std::any_cast<double>(value["Comm_Set_T"]);
	  if(it.first == "Comm_Set_Power")
		Comm_Set_Power = std::any_cast<double>(value["Comm_Set_Power"]);
	}
  }
  
 private:
  //Comm_Set_A Comm_Set_A
  double Comm_Set_A; 
  //Comm_Set_i Comm_Set_i
  int32_t Comm_Set_i; 
  //Comm_Set_azi Comm_Set_azi
  double Comm_Set_azi; 
  //Comm_Set_alt Comm_Set_alt
  double Comm_Set_alt; 
  //Comm_Set_Freq_Bw Comm_Set_Freq_Bw
  double Comm_Set_Freq_Bw; 
  //Comm_Set_Freq Comm_Set_Freq
  double Comm_Set_Freq; 
  //Comm_Set_T Comm_Set_T
  double Comm_Set_T; 
  //Comm_Set_Power Comm_Set_Power
  double Comm_Set_Power; 
};

// Comm_tran Comm_tran
class Comm_tran{
 public:
  virtual CSValueMap ToValueMap(){
	CSValueMap value;
	value.emplace("tran_pos", tran_pos);
    value.emplace("tran_p", tran_p);
    value.emplace("tran_gt", tran_gt);
    value.emplace("tran_tf", tran_tf);
    value.emplace("tran_B", tran_B);
	
	return std::move(value);
  }
  
  virtual void FromValueMap(CSValueMap &value){
	for(auto &it : value) {
	  if(it.first == "tran_pos")
	    tran_pos.FromValueMap(std::any_cast<CSValueMap>(value["tran_pos"]));
	  if(it.first == "tran_p")
		tran_p = std::any_cast<double>(value["tran_p"]);
	  if(it.first == "tran_gt")
		tran_gt = std::any_cast<double>(value["tran_gt"]);
	  if(it.first == "tran_tf")
		tran_tf = std::any_cast<double>(value["tran_tf"]);
	  if(it.first == "tran_B")
		tran_B = std::any_cast<double>(value["tran_B"]);
	}
  }
  
 private:
  //tran_pos tran_pos
  V3f tran_pos; 
  //tran_p tran_p
  double tran_p; 
  //tran_gt tran_gt
  double tran_gt; 
  //tran_tf tran_tf
  double tran_tf; 
  //tran_B tran_B
  double tran_B; 
};

