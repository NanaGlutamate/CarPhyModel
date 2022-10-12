#pragma once

#include "communication.hpp"
#include "../framework/pattern.hpp"

using CommunicationFactory = Factory<Communication>;

inline CommunicationFactory::LUT CommunicationFactory::look_up_table{};