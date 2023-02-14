#include "type_define_info.h"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"

using namespace rapidxml;
using namespace config;

bool CSTypeDefineInfo::Clear() {
    info.type_defines.clear();
    return true;
}

// 直接设置TypeDefineInfo
bool CSTypeDefineInfo::SetInfo(const TypeDefineInfo &info) {
    this->info = info;
    return true;
}

// 获得TypeDefineInfo
const TypeDefineInfo &CSTypeDefineInfo::GetInfo() { return info; }

bool CSTypeDefineInfo::ReadFile(const std::string &filename) {
    std::ifstream ifs(filename, std::ios_base::binary);
    if (!ifs)
        return false;
    return ReadContent(std::string(std::istreambuf_iterator<char>(ifs),
                                   std::istreambuf_iterator<char>()));
}

bool CSTypeDefineInfo::ReadContent(const std::string &content) {
    this->Clear();
    try {
        std::string tmp_content(content);
        xml_document<> doc;
        // 首先复制字符串 (rapidxml::xml_document<>::parse会改变字符串内容)
        doc.parse<0>((char *)tmp_content.c_str());

        xml_node<> *type_node = doc.first_node("Types");
        if (!type_node)
            return false;

        for (auto it = type_node->first_node("Type"); it;
             it = it->next_sibling()) {
            TypeDefine type;
            auto attribute = it->first_attribute("name");
            if (!attribute)
                return false;
            type.name = attribute->value();

            // 获取参数
            auto param_node = it->first_node("Params");
            if (!param_node)
                return false;

            // 获取类型具体信息
            for (auto it_p = param_node->first_node(); it_p;
                 it_p = it_p->next_sibling()) {
                auto att_name = it_p->first_attribute("name");
                auto att_type = it_p->first_attribute("type");
                auto att_unit = it_p->first_attribute("unit");

                if (!att_name || !att_type)
                    return false;

                type.params[att_name->value()] = att_type->value();
                if (att_unit)
                    type.param_unit[att_name->value()] = att_unit->value();
            }
            // 将获得的数据类型全部存储在types中
            info.type_defines[type.name] = type;
        }

    } catch (...) {
        return false;
    }
    return true;
}

bool CSTypeDefineInfo::WriteFile(const std::string &filename) {
    xml_document<char> doc;
    if (!WriteXmlDoc(doc))
        return false;
    std::ofstream out(filename);
    out << doc;
    out.close();
    return true;
}

bool CSTypeDefineInfo::WriteContent(std::string &content) {
    xml_document<char> doc;
    if (!WriteXmlDoc(doc))
        return false;
    rapidxml::print(std::back_inserter(content), doc, 0);
    return true;
}

bool CSTypeDefineInfo::WriteXmlDoc(xml_document<char> &doc) {
    xml_node<> *decl_node = doc.allocate_node(node_declaration);
    decl_node->append_attribute(doc.allocate_attribute("version", "1.0"));
    decl_node->append_attribute(doc.allocate_attribute("encoding", "UTF-8"));
    doc.append_node(decl_node);

    xml_node<> *Type_definition = doc.allocate_node(node_element, "Types");
    doc.append_node(Type_definition);

    for (const auto &[_, type_define] : info.type_defines) {
        xml_node<> *type_node = doc.allocate_node(node_element, "Type");
        type_node->append_attribute(
            doc.allocate_attribute("name", type_define.name.c_str()));
        xml_node<> *params = doc.allocate_node(node_element, "Params");

        auto &typeinfo = type_define.params;
        for (const auto &[param_name, param_type] : type_define.params) {
            xml_node<> *param = doc.allocate_node(node_element, "Param");
            param->append_attribute(
                doc.allocate_attribute("name", param_name.c_str()));
            param->append_attribute(
                doc.allocate_attribute("type", param_type.c_str()));
            auto unit = type_define.param_unit.find(param_name);
            if (unit != type_define.param_unit.end())
                param->append_attribute(
                    doc.allocate_attribute("unit", unit->second.c_str()));
            params->append_node(param);
        }

        type_node->append_node(params);
        Type_definition->append_node(type_node);
    }

    return true;
}