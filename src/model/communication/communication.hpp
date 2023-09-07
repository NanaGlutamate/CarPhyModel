#pragma once

#include "../tools/datastructure.hpp"

namespace carphymodel{

class Communication{
public:
    virtual bool sendMessage(const Vector3& self, const Vector3& target)=0;
    virtual ~Communication() = default;
};

}