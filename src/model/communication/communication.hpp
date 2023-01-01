#pragma once

namespace carPhyModel{

class Communication{
public:
    virtual void sendMessage()=0;
    virtual void receiveMessage()=0;
    virtual ~Communication() = default;
};

};