#ifndef CSCONFIG_CSTYPEDEFINEINFO_H
#define CSCONFIG_CSTYPEDEFINEINFO_H

#include <string>
#include <unordered_map>

#include "rapidxml/rapidxml.hpp"

namespace config {

// 自定义数据类型
struct TypeIdDefine {
    // 主题类型名称
    std::string name;
    // 参数名称-参数类型
    std::unordered_map<std::string, std::pair<uint32_t, std::string>> params;
};

// 自定义数据类型
struct TypeDefine {
    // 主题类型名称
    std::string name;
    // 参数名称-参数类型
    std::unordered_map<std::string, std::string> params;
    // 参数名称-参数单位
    std::unordered_map<std::string, std::string> param_unit;
};

// 数据类型定义信息
struct TypeDefineInfo {
    // 类型名称-类型信息
    std::unordered_map<std::string, TypeDefine> type_defines;
};

class CSTypeDefineInfo {
  public:
    // 解析文件并转换得到TypeDefineInfo
    bool ReadFile(const std::string &filename);
    // 解析文件内容并转换得到TypeDefineInfo
    bool ReadContent(const std::string &content);

    // 写入文件/字符串
    bool WriteFile(const std::string &filename);
    bool WriteContent(std::string &content);

    // 清空本地解析信息
    bool Clear();

    // 直接设置TypeDefineInfo
    bool SetInfo(const TypeDefineInfo &info);
    // 获得TypeDefineInfo
    const TypeDefineInfo &GetInfo();

  private:
    // 写入相关函数
    bool WriteXmlDoc(rapidxml::xml_document<char> &doc);

  private:
    TypeDefineInfo info;
};
} // namespace config

#endif // !CSCONFIG_CSTYPEDEFINEINFO_H
