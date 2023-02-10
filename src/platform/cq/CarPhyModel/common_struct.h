#include "model_struct.h"

// 本文件记录模型所使用的复杂数据结构

// BaseInfo 
// BaseInfo
class BaseInfo final : public CSParamBase {
 public:
  virtual CSValueMap ToValueMap() override {
    CSValueMap value;
    value.emplace("id", id);
    value.emplace("side", side);
    value.emplace("type", type);
    return value;
  }
  
  virtual void FromValueMap(CSValueMap &value) override {
    for(auto &it : value) {
      if(it.first == "id")
        id = std::any_cast<uint64_t>(it.second);
      if(it.first == "side")
        side = std::any_cast<uint16_t>(it.second);
      if(it.first == "type")
        type = std::any_cast<uint64_t>(it.second);
    }
  }
  
 public:
  // id
  uint64_t id; 
  // side
  uint16_t side; 
  // type
  uint64_t type; 
};

// Vector3 
// Vector3
class Vector3 final : public CSParamBase {
 public:
  virtual CSValueMap ToValueMap() override {
    CSValueMap value;
    value.emplace("x", x);
    value.emplace("y", y);
    value.emplace("z", z);
    return value;
  }
  
  virtual void FromValueMap(CSValueMap &value) override {
    for(auto &it : value) {
      if(it.first == "x")
        x = std::any_cast<double>(it.second);
      if(it.first == "y")
        y = std::any_cast<double>(it.second);
      if(it.first == "z")
        z = std::any_cast<double>(it.second);
    }
  }
  
 public:
  // x
  double x; 
  // y
  double y; 
  // z
  double z; 
};

// EntityInfo 
// EntityInfo
class EntityInfo final : public CSParamBase {
 public:
  virtual CSValueMap ToValueMap() override {
    CSValueMap value;
    value.emplace("baseInfo", baseInfo.ToValueMap());
    value.emplace("position", position.ToValueMap());
    value.emplace("velocity", velocity.ToValueMap());
    return value;
  }
  
  virtual void FromValueMap(CSValueMap &value) override {
    for(auto &it : value) {
      if(it.first == "baseInfo") {
        auto temp = std::any_cast<CSValueMap>(it.second);
        baseInfo.FromValueMap(temp);
      }
      if(it.first == "position") {
        auto temp = std::any_cast<CSValueMap>(it.second);
        position.FromValueMap(temp);
      }
      if(it.first == "velocity") {
        auto temp = std::any_cast<CSValueMap>(it.second);
        velocity.FromValueMap(temp);
      }
    }
  }
  
 public:
  // baseInfo
  BaseInfo baseInfo; 
  // position
  Vector3 position; 
  // velocity
  Vector3 velocity; 
};

// FireEvent 
// FireEvent
class FireEvent final : public CSParamBase {
 public:
  virtual CSValueMap ToValueMap() override {
    CSValueMap value;
    value.emplace("position", position.ToValueMap());
    value.emplace("range", range);
    value.emplace("target", target.ToValueMap());
    value.emplace("velocity", velocity.ToValueMap());
    value.emplace("weaponName", weaponName);
    return value;
  }
  
  virtual void FromValueMap(CSValueMap &value) override {
    for(auto &it : value) {
      if(it.first == "position") {
        auto temp = std::any_cast<CSValueMap>(it.second);
        position.FromValueMap(temp);
      }
      if(it.first == "range")
        range = std::any_cast<double>(it.second);
      if(it.first == "target") {
        auto temp = std::any_cast<CSValueMap>(it.second);
        target.FromValueMap(temp);
      }
      if(it.first == "velocity") {
        auto temp = std::any_cast<CSValueMap>(it.second);
        velocity.FromValueMap(temp);
      }
      if(it.first == "weaponName")
        weaponName = std::any_cast<std::string>(it.second);
    }
  }
  
 public:
  // position
  Vector3 position; 
  // range
  double range; 
  // target
  Vector3 target; 
  // velocity
  Vector3 velocity; 
  // weaponName
  std::string weaponName; 
};

