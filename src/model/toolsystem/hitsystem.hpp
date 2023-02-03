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
        auto& opt_hitbox = c.getSpecificSingletonComponent<Sphere>();
        if(!opt_hitbox.has_value()){
            double r = 0.;
            for(auto&& [id, _coordinate, _size] : c.getNormalComponents<Coordinate, Block>()){
                r = max(r, _coordinate.position.norm() + max(_size[0], max(_size[1], _size[2])));
            }
            opt_hitbox = Sphere{r};
        }
        auto& hitbox = opt_hitbox.value();
        //TODO:
    };
    virtual ~HitSystem() = default;
};

}