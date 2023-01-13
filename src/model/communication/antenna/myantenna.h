#pragma once

#include "../communication.hpp"

namespace carPhyModel{

class MyAntenna : public Communication{
public:
    MyAntenna() = default;
    virtual bool sendMessage() override;
    virtual ~MyAntenna() = default;
};

}