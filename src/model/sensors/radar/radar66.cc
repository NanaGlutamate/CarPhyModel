#include "radar66.h"
#include <iostream>
#include "Jammer.h"
#include<string>
#include "Radar.h"



//bool Radar66::Init(
//  const std::unordered_map<std::string, std::any> &value
//) {
//  if(auto it = value.find("JammerVel"); it != value.end()) {
//	JammerVel_.FromValueMap(std::any_cast<CSValueMap>(it->second));
//  }
//  if(auto it = value.find("JammerPos"); it != value.end()) {
//	JammerPos_.FromValueMap(std::any_cast<CSValueMap>(it->second));
//  }
//  if(auto it = value.find("ForceSideID"); it != value.end()) {
//  	ForceSideID_ = std::any_cast<uint16_t>(it->second);
//  }
//  if(auto it = value.find("ID"); it != value.end()) {
//  	ID_ = std::any_cast<uint64_t>(it->second);
//  }
//  if(auto it = value.find("InstanceName"); it != value.end()) {
//  	InstanceName_ = std::any_cast<std::string>(it->second);
//  }
//  if(auto it = value.find("JammerFreRange"); it != value.end()) {
//  	JammerFreRange_ = std::any_cast<double>(it->second);
//  }
//  if(auto it = value.find("JammerFre"); it != value.end()) {
//  	JammerFre_ = std::any_cast<double>(it->second);
//  }
//  if(auto it = value.find("TargetRCS"); it != value.end()) {
//  	TargetRCS_ = std::any_cast<double>(it->second);
//  }
//  if(auto it = value.find("ModelID"); it != value.end()) {
//  	ModelID_ = std::any_cast<std::string>(it->second);
//  }
//  if(auto it = value.find("RadarBand"); it != value.end()) {
//  	RadarBand_ = std::any_cast<double>(it->second);
//  }
//  if(auto it = value.find("RadarCenter"); it != value.end()) {
//  	RadarCenter_ = std::any_cast<double>(it->second);
//  }
//
//  state_ = CSInstanceState::IS_INITIALIZED;
//  WriteLog("Radar66 model Init", 1);
//  return true;
//}
//
//bool Radar66::Tick(double time) {
//  params_.clear();
//  
//  // 此处填写模型单步运算逻辑
//  // 需输出的参数应通过emplace方法写入params_
//  // 等候GetOutput接口被调用时参数对外部输出
//  
//  RADARSTATE RadarState;
//  RADARSTATE JammerState;
//  TARGETSTATE TargetState;
//
//  RadarState.rPos.x = std::any_cast<double>(RadarPos_.ToValueMap()["x"]);
//  RadarState.rPos.y = std::any_cast<double>(RadarPos_.ToValueMap()["y"]);
//  RadarState.rPos.z = std::any_cast<double>(RadarPos_.ToValueMap()["z"]);
//  RadarState.rVel.x = std::any_cast<double>(RadarVel_.ToValueMap()["x"]);
//  RadarState.rPos.y = std::any_cast<double>(RadarVel_.ToValueMap()["y"]);
//  RadarState.rVel.z = std::any_cast<double>(RadarVel_.ToValueMap()["z"]);
//  RadarState.Height = RadarState.rPos.y;
//  RadarState.fre_band = RadarBand_;
//  RadarState.fre_center = RadarCenter_;
//
//  TargetState.tPos.x = std::any_cast<double>(TargetPos_.ToValueMap()["x"]);
//  TargetState.tPos.y = std::any_cast<double>(TargetPos_.ToValueMap()["y"]);
//  TargetState.tPos.z = std::any_cast<double>(TargetPos_.ToValueMap()["z"]);
//  TargetState.tVel.x = std::any_cast<double>(TargetVel_.ToValueMap()["x"]);
//  TargetState.tPos.y = std::any_cast<double>(TargetVel_.ToValueMap()["y"]);
//  TargetState.tVel.z = std::any_cast<double>(TargetVel_.ToValueMap()["z"]);
//  TargetState.RCS = TargetRCS_;
//
//  JammerState.rPos.x = std::any_cast<double>(JammerPos_.ToValueMap()["x"]);
//  JammerState.rPos.y = std::any_cast<double>(JammerPos_.ToValueMap()["y"]);
//  JammerState.rPos.z = std::any_cast<double>(JammerPos_.ToValueMap()["z"]);
//  JammerState.rVel.x = std::any_cast<double>(JammerVel_.ToValueMap()["x"]);
//  JammerState.rPos.y = std::any_cast<double>(JammerVel_.ToValueMap()["y"]);
//  JammerState.rVel.z = std::any_cast<double>(JammerVel_.ToValueMap()["z"]);
//  double JammerFre = JammerFre_;
//  double JammerFreRange = JammerFreRange_;
//
//  Jammer Jam1;
//  Jam1.Init();
//  Jam1.InitValue("JPos", &JammerState.rPos);
//  Jam1.InitValue("JVel", &JammerState.rVel);
//  Jam1.InitValue("JFrequency", &JammerFre);
//  Jam1.InitValue("JFreqRange", &JammerFreRange);
//  std::unordered_map<std::string, std::any> setinput;
//  setinput = { {"Target_Position",TargetState.tPos},{"Target_Velocity",TargetState.tVel},
//  {"Radar_Position",RadarState.rPos},{"Radar_Velocity",RadarState.rVel},
//  {"Radar_FreCenter",RadarState.fre_center},{"Radar_FreBand",RadarState.fre_band} };
//  Jam1.SetInput(setinput);
//  //cout << Jam1.CalcJPower() << endl;
//  //cout << Jam1.GetState() << endl;
//
//  double P = Jam1.CalcJPower();
//  Radar PdRadar1;
//  PdRadar1.Init();
//  PdRadar1.InitValue("RPos", &RadarState.rPos);
//  PdRadar1.InitValue("RVel", &RadarState.rVel);
//  std::unordered_map<std::string, std::any> setinput2;
//  setinput2 = { {"Target_Position",TargetState.tPos},{"Target_Velocity",TargetState.tVel} };
//  PdRadar1.SetInput(setinput2);
//  PdRadar1.Set_JamPower(P);
//  PdRadar1.update();
//  cout << PdRadar1.Get_detect_result() << endl;
//  detect_result_ = PdRadar1.Get_detect_result();
//
//  WriteLog("Radar66 model Tick", 1);
//  return true;
//}
//
//bool Radar66::SetInput(
//  const std::unordered_map<std::string, std::any> &value
//) {
//  if(auto it = value.find("RadarPos"); it != value.end()) {
//	RadarPos_.FromValueMap(std::any_cast<CSValueMap>(it->second));
//  }
//  if(auto it = value.find("RadarVel"); it != value.end()) {
//	RadarVel_.FromValueMap(std::any_cast<CSValueMap>(it->second));
//  }
//  if(auto it = value.find("TargetPos"); it != value.end()) {
//	TargetPos_.FromValueMap(std::any_cast<CSValueMap>(it->second));
//  }
//  if(auto it = value.find("TargetVel"); it != value.end()) {
//	TargetVel_.FromValueMap(std::any_cast<CSValueMap>(it->second));
//  }
//
//  WriteLog("Radar66 model SetInput", 1);
//  return true;
//}
//
//std::unordered_map<std::string, std::any>* Radar66::GetOutput() {
//  state_ = CSInstanceState::IS_RUNNING;
//  params_.emplace("ForceSideID", GetForceSideID());
//  params_.emplace("ModelID", GetModelID());
//  params_.emplace("InstanceName", GetInstanceName());
//  params_.emplace("ID", GetID());
//  params_.emplace("State", uint16_t(GetState()));
//
//  params_.emplace("detect_result", detect_result_);
//  
//  WriteLog("Radar66 model GetOutPut", 1);
//  return &params_;
//}

//RADAR66_EXPORT CSModelObject* CreateModelObject() {
//  CSModelObject* model = new Radar66();
//  return model;
//}
//
//RADAR66_EXPORT void DestroyMemory(void* mem, bool is_array) {
//  if(is_array) {
//    delete[]((Radar66*)mem);
//  } else {
//    delete ((Radar66*)mem);
//  }
//}
