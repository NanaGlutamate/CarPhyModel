#pragma once

#include "../communication.hpp"
#include "extern/Comm.h"

namespace carPhyModel{

class MyAntenna : public Communication{
public:
    virtual void sendMessage() override {};
    virtual void receiveMessage() override {};
    virtual ~MyAntenna() = default;
};

}