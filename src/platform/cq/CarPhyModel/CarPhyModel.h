#pragma once

#ifdef _WIN32
#include <Windows.h>
#include <atlbase.h>
#include <atlwin.h>
#else
#include <dlfcn.h>
#endif

#include <string>
#include "csmodel_base.h"
#include "common_struct.h"
#include "CarPhyModel_export.h"
#include "src/model/carmodel.hpp"

extern "C"
{
    CARPHYMODEL_EXPORT CSModelObject *CreateModelObject();
    CARPHYMODEL_EXPORT void DestroyMemory(void *mem, bool is_array);
}

// CarPhyModel
class CarPhyModel : public CSModelObject
{
public:
    // 初始化
    virtual bool Init(const std::unordered_map<std::string, std::any> &value)
        override;
    // 单步运算 time 应推进的步长(ms)
    virtual bool Tick(double time) override;
    // 获取输入参数
    virtual bool SetInput(const std::unordered_map<std::string, std::any> &value)
        override;
    // 对外部输出模型参数
    virtual std::unordered_map<std::string, std::any> *GetOutput() override;

private:
    carphymodel::CarModel model;
    // 模型动态库(dll/so)所在路径
    std::string library_dir_;
};
