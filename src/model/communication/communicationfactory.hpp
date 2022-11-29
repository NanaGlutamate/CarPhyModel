#pragma once

#include "communication.hpp"
#include "../framework/pattern.hpp"

namespace carPhyModel{

class CommunicationFactory : public Factory<Communication>{};

inline CommunicationFactory::LUT CommunicationFactory::look_up_table{};

};