#include "carsensor.h"

#include <algorithm>
#include <tuple>
#include "../tools/datastructure.hpp"
#include "../sensors/sensorfactory.hpp"
#include "../tools/myrandom.hpp"

namespace{

};

namespace carphymodel{

using namespace carphymodel::command;

void SensorSystem::tick(double dt, Components& c)
{
    double& jammer = get<1>(*c.getNormal<ProtectionModel>().begin()).jammer;//Լ����һ��entity��ӳ��������
    for (auto&& [k, v] : c.getSpecificSingleton<CommandBuffer>().value()) {
        auto [param1, param2] = any_cast<std::tuple<double, double>>(v);
        if (k == COMMAND_TYPE::ACTIVATE_INTERFERE) {
        jammer = param1;
        }
    }
    auto& mem = c.getSpecificSingleton<ScannedMemory>().value();
    auto& baseCoordinate = c.getSpecificSingleton<Coordinate>().value();
    auto& hull = c.getSpecificSingleton<Hull>().value();
    //auto& protect = get<1>(c.getNormal<ProtectionModel>().begin());
    for(auto& x : mem){
        std::get<0>(x.second) += dt;
    }
    int tmprange = 0;
    SensorData bestSensor;
    for(auto&& [id, _sensor, _damage] : c.getNormal<SensorData, DamageModel>()){
        if(_damage.damageLevel == DAMAGE_LEVEL::K || _damage.damageLevel == DAMAGE_LEVEL::KK){
            continue;
        }
        if (_sensor.detectrange > tmprange)
        {
            bestSensor = _sensor;
            tmprange = _sensor.detectrange;
        }
        auto sensor = SensorFactory::getProduct(_sensor.type);
        for(auto&& [vid, _entityInfo] : mem){
            if(std::get<0>(_entityInfo) != 0. && 
                sensor->isDetectable(baseCoordinate, std::get<1>(_entityInfo), _sensor, hull))
            {
                std::get<0>(_entityInfo) = 0.;
            }
        }
    }
    //����ScannedMemory��ֵ��pos)
    //auto sensor = SensorFactory::getProduct(bestSensor.type);
    for (auto&& [vid, _entityInfo] : mem) {
        //���ݾ����pos���Ӳ�ͬ��ƫ��
        if (std::get<0>(_entityInfo)== 0.) {
            double distance = (baseCoordinate.position - std::get<1>(_entityInfo).position).norm();
            double maxDeviation = bestSensor.target_positioning_accuracy * distance / bestSensor.detectrange;
                //std::min(bestSensor.target_positioning_accuracy * bestSensor.detectrange / distance,bestSensor.detectrange - distance);
            double angle = carphymodel::rand() * 2 * PI;
            std::get<1>(mem[vid]).position.x += maxDeviation * std::cos(angle);
            std::get<1>(mem[vid]).position.y += maxDeviation * std::sin(angle);
        }
    }
    auto& detectrange = c.getSpecificSingleton<EventBuffer>().value();
    //��̽�ⷶΧ��Զ�Ĵ�������̽�ⷶΧ����EventBuffer
    detectrange.emplace("maxdetectrange", bestSensor.detectrange);
    //����SystemScannedMemory��ֵ����ScannedMemory��ֵ���������ɨ�赽�����岢����̽�⵽��ϵͳ������������̽�⵽����Ŀ����뵽ScannedMemory��
    /*auto& mem_sys = c.getSpecificSingleton<SystemScannedMemory>().value();
    for (auto&& [vid, _entityInfo] : mem_sys) {
        if (std::get<0>(mem[vid]) != 0.) {
            std::get<0>(mem[vid]) = 0.;
        }
    }
    mem_sys.clear();*/
    return;
}

}