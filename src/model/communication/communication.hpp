#pragma once

namespace carPhyModel{

class Communication{
public:
    virtual bool sendMessage()=0;
    virtual ~Communication() = default;
};

}