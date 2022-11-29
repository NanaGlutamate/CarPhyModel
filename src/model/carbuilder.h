#pragma once

#include <string>
#include "carmodel.h"

namespace carPhyModel{

class CarBuilder{
private:

public:
    CarBuilder(){};

    //! build model from XML file
    //! @param srcXML string of content of XML file
    //! @param model target model to build, will be erased if--
    //! @param need_erase is 'true' (false by default)
    //! @return none.
    void build(const std::string& srcXML, CarModel& model, bool need_erase=false);
};

}