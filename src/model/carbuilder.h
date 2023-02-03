#pragma once

#include <string>
#include <fstream>
#include "carmodel.hpp"

namespace carphymodel{

class CarBuilder{
public:
    //! build model from XML file
    //! @param srcXML string of content of XML file
    //! @param model target empty model to build
    //! @return none.
    static void buildFromSource(const std::string& srcXML, CarModel& model);

    //! build model from XML file
    //! @param XMLFilePath string of path of XML file
    //! @param model target empty model to build
    //! @return none.
    static void buildFromFile(const std::string& XMLFilePath, CarModel& model){
        using namespace std;
        std::ifstream file(XMLFilePath);
        std::string buffer;    
        while(!(file.eof())){
            string tmp;
            getline(file, tmp);
            buffer.append(tmp);
        }
        buildFromSource(buffer, model);
    }
};

}