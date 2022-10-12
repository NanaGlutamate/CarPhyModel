#ifndef __SRC_MODEL_CARPROTECTION_H__
#define __SRC_MODEL_CARPROTECTION_H__

#include "tools/datastructure.hpp"

class ProtectionSystem{
public:
    ProtectionSystem(){};
    void tick(double dt, ProtectionList& protection, const CoordinateList& coordinate);
};

#endif