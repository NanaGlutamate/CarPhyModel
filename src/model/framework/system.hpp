#pragma once

#include <string>
#include "../tools/datastructure.hpp"

namespace carphymodel{

struct SystemRestriction{
    std::vector<std::string> thisBefore;
    std::vector<std::string> thisAfter;
};

class System{
public:
    // virtual std::string_view getTag(){return std::string_view();};
    // virtual SystemRestriction& getConstraint(){static SystemConstraint sc; return sc;};
    virtual void tick(double dt, Components& c)=0;
    virtual ~System() = default;
};

};