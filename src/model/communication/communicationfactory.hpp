#pragma once

#include "communication.hpp"
#include "../framework/pattern.hpp"

namespace carPhyModel{

struct CommunicationFactory : public Factory<CommunicationFactory, Communication>{
    inline static LUT look_up_table{};
};

};