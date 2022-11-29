#pragma once

namespace carPhyModel{

class Communication{
public:
    virtual void sendMessage()=0;
    virtual ~Communication(){};
};

};