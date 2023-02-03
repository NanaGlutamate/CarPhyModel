#pragma once

#include "../communication.hpp"

namespace carphymodel{

class MyAntenna : public Communication{
public:
    MyAntenna() = default;
    virtual bool sendMessage() override;
    virtual ~MyAntenna() = default;
};

}