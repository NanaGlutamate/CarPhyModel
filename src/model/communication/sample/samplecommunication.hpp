#pragma once

#include "../communication.hpp"

namespace carPhyModel{

class SampleCommunication : public Communication{
public:
    SampleCommunication(double range): range(range){};
    virtual bool sendMessage() override {};
    virtual ~SampleCommunication() = default;
private:
    double range;
};

}