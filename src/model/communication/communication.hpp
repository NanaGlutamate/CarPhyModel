#pragma once

namespace carphymodel{

class Communication{
public:
    virtual bool sendMessage()=0;
    virtual ~Communication() = default;
};

}