#pragma once

#include <cmath>

#include "../tools/datastructure.hpp"
#include "../framework/system.hpp"

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
            opt_hitbox = Sphere{r * 1.5};
        }
        auto& hitbox = opt_hitbox.value();
        for(auto&& _fireEvent : c.getSpecificSingleton<FireEventQueue>().value()){
            auto& carPosition = c.getSpecificSingleton<Coordinate>()->position;
            auto direction = _fireEvent.target - _fireEvent.position;
            auto distance = carPosition - _fireEvent.position;
            // no need to filter out the event that is fired by this car, cause it's already done by CarPhyModel::SetInput
            if(((distance).out(direction)).norm() / direction.norm() < hitbox.r){
                c.getSpecificSingleton<HitEventQueue>()->push_back(std::move(_fireEvent));
            }
        }
        c.getSpecificSingleton<FireEventQueue>()->clear();
    }
    virtual ~HitSystem() = default;
};

}