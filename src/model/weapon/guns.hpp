#ifndef __GUNS_MODEL_H__
#define __GUNS_MODEL_H__

#include "damage.hpp"
#include <unordered_map>
#include <any>
#include <string>
#include <random>
#include <functional>
#include <cmath>

struct FireCommand{
    Position target_position;
    CSValueMap ToValueMap() const{
        return CSValueMap{
            {"target_position", target_position.ToValueMap()},
        };
    };
    void FromValueMap(const CSValueMap &value){
        for(auto& it : value){
            if(it.first == "target_position")target_position.FromValueMap(std::any_cast<CSValueMap>(it.second));
        }
    };
};

class Guns{
public:
    enum class State{
        AVAILABLE=0,
        RELOADING=1,
        RANOUT=2,
    };
    using DataContainer = std::unordered_map<std::string, std::any>;
    DataContainer output_buffer;
    FireData fire_data;
    Position self_position;
    bool is_fire;
    unsigned int ammo_remain;
    State state;
    double reloading_time;
    double reloading_time_remain;
    double piercing_ability;
    double ammo_speed;
    double CEP_1km;
    double range;
    std::default_random_engine generator;
    void Init(const DataContainer& data){
        output_buffer.clear();
        //重新填装时间，单位为秒
        if(auto it = data.find("reloading_time");it!=data.end()){
            reloading_time = std::any_cast<double>(it->second);
        }
        if(auto it = data.find("ammo_remain");it!=data.end()){
            ammo_remain = std::any_cast<unsigned int>(it->second);
        }
        // 穿甲能力，单位：米
        if(auto it = data.find("piercing_ability");it!=data.end()){
            piercing_ability = std::any_cast<double>(it->second);
        }
        if(auto it = data.find("ammo_speed");it!=data.end()){
            ammo_speed = std::any_cast<double>(it->second);
        }
        // 1km圆概率误差
        if(auto it = data.find("CEP_1km");it!=data.end()){
            CEP_1km = std::any_cast<double>(it->second);
        }
        if(auto it = data.find("range");it!=data.end()){
            range = std::any_cast<double>(it->second);
        }
        reloading_time_remain = 0.;
        if(ammo_remain > 0)state=State::AVAILABLE;
        else state=State::RANOUT;
        is_fire = false;
    };
    DataContainer* GetOutput(){
        output_buffer.clear();
        //状态：可开火=0，正在填装=1，无剩余子弹=2
        output_buffer.emplace("state", int(state));
        //是否开火
        output_buffer.emplace("is_fire", int(is_fire));
        if(is_fire){
            output_buffer.emplace("fire_data", fire_data.ToValueMap());
        }
        return &output_buffer;
    };
    void SetInput(const DataContainer& data){
        is_fire = false;
        if(auto it = data.find("self_position");it!=data.end()){
            self_position.FromValueMap(std::any_cast<CSValueMap>(it->second));
        }
        if(auto it = data.find("fire_command");it!=data.end()){
            if(state == State::AVAILABLE){
                FireCommand command;
                command.FromValueMap(std::any_cast<CSValueMap>(it->second));
                if(distance(self_position, command.target_position) <= range){
                    is_fire = true;
                    ammo_remain--;
                    reloading_time_remain = reloading_time;
                    state = State::RELOADING;
                    std::normal_distribution<double> distribution(0., (CEP_1km / 1000.) * distance(self_position, command.target_position) / 1.17741/*sqrt(2ln(2))*/);
                    Position tmp = command.target_position;
                    tmp.altitude += distribution(generator);
                    if(tmp.latitude >= -89.999 && tmp.latitude <= 89.999)tmp.longitude += distribution(generator) / (111000 * cos(tmp.latitude * DEG2RAD));
                    tmp.latitude += distribution(generator) / 111000;
                    fire_data = {
                        tmp,
                        self_position,
                        ammo_speed,
                        piercing_ability,
                        0,
                    };
                }
            }
        }
    };
    void Tick(double dt){
        reloading_time_remain -= dt;
        if(reloading_time_remain <= 0.){
            reloading_time_remain = 0.;
            if(ammo_remain > 0)state=State::AVAILABLE;
            else state=State::RANOUT;
        }
    };
};

#endif