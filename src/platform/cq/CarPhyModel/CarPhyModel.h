#pragma once

#ifdef _WIN32
#include <Windows.h>
#include <atlbase.h>
#include <atlwin.h>
#else
#include <dlfcn.h>
#endif

#include <string>
#include "../csmodel_base/csmodel_base.h"
#include "common_struct.h"
#include "CarPhyModel_export.h"
#include "src/model/carmodel.hpp"

extern "C"
{
    CARPHYMODEL_EXPORT CSModelObject *CreateModelObject();
    CARPHYMODEL_EXPORT void DestroyMemory(void *mem, bool is_array);
}


class CarPhyModel : public CSModelObject
{
public:
    virtual bool Init(const std::unordered_map<std::string, std::any> &value)
        override;
    
    virtual bool Tick(double time) override;
    
    virtual bool SetInput(const std::unordered_map<std::string, std::any> &value)
        override;
    
    virtual std::unordered_map<std::string, std::any> *GetOutput() override;

    // deg
    struct Location{
        double longitude, latitude, altitude;
    } location;
    carphymodel::CarModel model;
};
