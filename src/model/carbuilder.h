#ifndef __SRC_MODEL_CARBUILDER_H__
#define __SRC_MODEL_CARBUILDER_H__

#include <string>
#include <algorithm>
#include "../extern/rapidxml-1.13/rapidxml.hpp"
#include "carprotection.h"
#include "carmodel.h"

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

#endif