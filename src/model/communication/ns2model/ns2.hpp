#pragma once

#include "../communication.hpp"

namespace wsfplugin {

struct NS2Model : public carphymodel::Communication {
    double params;
    virtual bool sendMessage(const carphymodel::Vector3& self, const carphymodel::Vector3& target){
        // TODO:
        
    }
    virtual ~NS2Model() = default;
};

} // namespace wsfplugin
