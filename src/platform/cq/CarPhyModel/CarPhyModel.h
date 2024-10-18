#pragma once

#ifdef _WIN32
#include <Windows.h>
#include <atlbase.h>
#include <atlwin.h>
#else
#include <dlfcn.h>
#endif

#include <atomic>
#include <string>
#include <mutex>
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
    CarPhyModel() = default;
    virtual bool Init(const std::unordered_map<std::string, std::any> &value)
        override;
    
    virtual bool Tick(double time) override;
    
    virtual bool SetInput(const std::unordered_map<std::string, std::any> &value)
        override;
    
    virtual std::unordered_map<std::string, std::any> *GetOutput() override;
    // avoid data race through atomic var
    // inline static volatile std::atomic<carphymodel::VID> VIDCounter = 0;
    inline static carphymodel::VID VIDCounter = 0;
    // need lock?
    inline static std::mutex initLock;
    carphymodel::VID myVID;
    carphymodel::VID getVID() {
        return myVID;
    }

    // deg
    inline static struct Location {
        double longitude, latitude, altitude;
    } location;
    carphymodel::CarModel model;
    double timeiter = 0;
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    //for test
    std::vector<std::any> lasttimemessage = {};
    bool areVectorsEqual(const std::vector<std::any>& vec1, const std::vector<std::any>& vec2) {
        if (vec1.size() != vec2.size()) {
            return false;
        }
        // 逐个比较向量中的元素
        for (size_t i = 0; i < vec1.size(); ++i) {
            if (vec1[i].type() != typeid(std::string) || vec2[i].type() != typeid(std::string)) {
                return false; // 类型不是 std::string，不相等
            }
            if (std::any_cast<std::string>(vec1[i]) != std::any_cast<std::string>(vec2[i])) {
                return false; // std::string 类型不相等
            }
        }
        return true;
    }
};
