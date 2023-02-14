#ifndef CSCONFIG_CSMODELINFO_H
#define CSCONFIG_CSMODELINFO_H

#include <string>
#include <unordered_map>

#include "rapidxml/rapidxml.hpp"

namespace config {

// 模型参数
struct ModelParamItem {
    // 参数名称
    std::string name;
    // 参数类型
    std::string type;
    // 显示名称
    std::string display_name;
    // 用途
    std::string usage;
    // 数值
    std::string value;
    // 单位
    std::string unit;
};

// 模型描述文件内容-模型动态库-模型依赖项
using ModelPackage = std::tuple<std::string, std::string, std::string>;

struct ModelInfo {
    // 模型类型ID (UUID)
    std::string id;
    std::string name;
    std::string display_name;
    // 模型对应dll名称
    std::string dll_name;
    std::string category;
    // 是否有地理信息
    bool has_geo_info;
    // 仿真步长倍数
    std::string multiples;
    // 模型参数
    std::unordered_map<std::string, ModelParamItem> params;
};

class CSModelInfo {
  public:
    bool ReadFile(const std::string &config_filepath);
    bool ReadContent(const std::string &config_content);

    bool WriteFile(const std::string &config_filepath);
    bool WriteContent(std::string &config_content);

    // 清空本地解析的信息
    bool Clear();

    // 获得结构体数据
    const ModelInfo &GetInfo();
    bool SetInfo(const ModelInfo &info);

  private:
    bool ParseModelInfo(rapidxml::xml_node<char> *&model_node);
    bool WriteXmlDoc(rapidxml::xml_document<char> &doc);

  private:
    ModelInfo info;
};

} // namespace config

#endif // !CSCONFIG_CSMODELINFO_H
