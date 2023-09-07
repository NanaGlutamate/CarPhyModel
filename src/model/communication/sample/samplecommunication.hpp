#pragma once

#include "../communication.hpp"

namespace carphymodel{

class SimpleCommunication : public Communication{
public:
    SimpleCommunication(double range): range(range){};
    virtual bool sendMessage(const Vector3& self, const Vector3& target) override{
        return (self - target).norm() <= range;
    };
    virtual ~SimpleCommunication() = default;
private:
    double range;
};

}