#pragma once

#include "communication.hpp"
#include "../framework/pattern.hpp"
#include "antenna/myantenna.h"

namespace carphymodel{

struct CommunicationFactory : public Factory<CommunicationFactory, Communication>{
    inline static LUT look_up_table{{"antenna", std::make_shared<MyAntenna>()}};
};

};