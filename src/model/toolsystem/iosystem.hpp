#pragma once

#include "../tools/datastructure.hpp"
#include "../framework/system.hpp"

namespace carphymodel{

//using namespace carphymodel::command;
class PrepareSystem : public System{
public:
    PrepareSystem() = default;
    virtual void tick(double dt, Components& c) override {
        c.getSpecificSingleton<EventBuffer>().value().clear();
        //for (auto&& [k, v] : c.getSpecificSingleton<CommandBuffer>().value()) {
        //    auto [param1, param2] = any_cast<std::tuple<double, double>>(v);
        //    if (k == COMMAND_TYPE::REPAIR) {
        //        //
        //        auto& damage = c.getSpecificSingleton<DamageModel>().value();
        //        if (damage.damageLevel == DAMAGE_LEVEL::K || damage.damageLevel == DAMAGE_LEVEL::KK) {
        //            return;
        //        }
        //    }
        //}
    };
    virtual ~PrepareSystem() = default;
};

class OutputSystem : public System{
public:
    OutputSystem() = default;
    virtual void tick(double dt, Components& c) override {
        c.getSpecificSingleton<CommandBuffer>().value().clear();
    };
    virtual ~OutputSystem() = default;
};

}
