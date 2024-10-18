#pragma once

#include <cmath>

#include "../tools/datastructure.hpp"
#include "../framework/system.hpp"
#include "../tools/myrandom.hpp"

namespace carphymodel{

class HitSystem : public System{
public:
    HitSystem() = default;
    virtual void tick(double dt, Components& c) override {
        using std::max;
        auto& opt_hitbox = c.getSpecificSingleton<Sphere>();
        if(!opt_hitbox.has_value()){
            double r = 0.;
            for(auto&& [id, _coordinate, _size] : c.getNormal<Coordinate, Block>()){
                r = max(r, _coordinate.position.norm() + max(_size[0], max(_size[1], _size[2])));
            }
            //change by wsb from 1.5 to 2.5
            opt_hitbox = Sphere{r * 2.5};
        }
        auto& hitbox = opt_hitbox.value();
        auto& carPosition = c.getSpecificSingleton<Coordinate>()->position;
        for(auto&& _fireEvent : c.getSpecificSingleton<FireEventQueue>().value()){
            // auto direction = _fireEvent.target - _fireEvent.position;
            // auto distance = carPosition - _fireEvent.position;
            // // no need to filter out the event that is fired by this car, cause it's already done by CarPhyModel::SetInput
            // if(((distance).out(direction)).norm() / direction.norm() < hitbox.r){
            //     c.getSpecificSingleton<HitEventQueue>()->push_back(std::move(_fireEvent));
            // }

            
            //add by wsb: add random rate to refer hit rate, which is related to 1.whether is the first fire;2.the params of the weapon
            if (_fireEvent.weaponName == "AP" || _fireEvent.weaponName == "HE" ||
                _fireEvent.weaponName == "antitankmissile")
            {
                //静止时取参数1，运动时取参数2
                double tmpdis =
                    c.getSpecificSingleton<Hull>().value().velocity.norm() == 0 ? _fireEvent.param1 : _fireEvent.param2;
                double tmprate = _fireEvent.weaponName == "antitankmissile" ? 0.75 : 0.45;
                double firstrate = tmprate + _fireEvent.isFirst * 0.1 + (tmpdis - _fireEvent.range) / tmpdis;
                if (firstrate < 0.1) firstrate = 0.1;
                if (firstrate > 0.9) firstrate = 0.9;
                if (carphymodel::rand() > firstrate) break;
            }
            if (_fireEvent.weaponName == "verticalmissile")
            {
                //参数1是速度，参数2是命中率
                double tmpvel = c.getSpecificSingleton<Hull>().value().velocity.norm() * 3.6;
                double firstrate = _fireEvent.param2 + _fireEvent.isFirst * 0.05 + (_fireEvent.param1 - tmpvel) / _fireEvent.param1;
                if (firstrate < 0.1)
                    firstrate = 0.1;
                if (firstrate > 0.9)
                    firstrate = 0.9;
                if (carphymodel::rand() > firstrate)
                    break;
            }

            if ((carPosition - _fireEvent.target).norm() < hitbox.r) {
                c.getSpecificSingleton<HitEventQueue>()->push_back(std::move(_fireEvent));
            }
        }
        c.getSpecificSingleton<FireEventQueue>()->clear();
    }
    virtual ~HitSystem() = default;
};

}