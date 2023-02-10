#include <filesystem>
#include <fstream>

#include "model_info.h"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

namespace fs = std::filesystem;

using namespace rapidxml;
using namespace config;

bool CSModelInfo::Clear() {
  info.params.clear();
  return true;
}

// 获得结构体数据
const ModelInfo & CSModelInfo::GetInfo() {
  return info;
}

bool CSModelInfo::SetInfo(const ModelInfo & info) {
  this->info = info;
  std::string config_content;
  return WriteContent(config_content);
}

bool CSModelInfo::ReadFile(const std::string& config_filepath) {
  std::ifstream ifs(config_filepath, std::ios::binary);
  if (!ifs)
    return false;
  std::string config_content = std::string(std::istreambuf_iterator<char>(ifs),
                               std::istreambuf_iterator<char>());
  return ReadContent(config_content);
}

bool CSModelInfo::ReadContent(const std::string& config_content) {
  Clear();

  std::string tmp_content(config_content);
  xml_document<> doc;
  // 首先复制字符串 (rapidxml::xml_document<>::parse会改变字符串内容)
  doc.parse<0>((char*)tmp_content.data());

  xml_node<> *model_node = doc.first_node("ModelInfo");
  if (!model_node)
    return false;

  // 获取模型基本属性及参数
  return ParseModelInfo(model_node);
}

bool CSModelInfo::WriteFile(const std::string& config_filepath) {
  xml_document<char> doc;
  if (!WriteXmlDoc(doc))
    return false;
  std::ofstream out(config_filepath);
  out << doc;
  out.close();
  return true;
}

bool CSModelInfo::WriteContent(std::string& config_content) {
  xml_document<char> doc;
  if (!WriteXmlDoc(doc))
    return false;
  rapidxml::print(std::back_inserter(config_content), doc, 0);
  return true;
}

bool CSModelInfo::ParseModelInfo(rapidxml::xml_node<char>* &model_node) {
  try {
    auto att_id = model_node->first_attribute("id");
    auto att_name = model_node->first_attribute("name");
    auto att_display_name = model_node->first_attribute("displayName");
    auto att_dll = model_node->first_attribute("dllName");
    auto att_geo = model_node->first_attribute("hasGeoInfo");
    auto att_category = model_node->first_attribute("category");
    auto att_multiples = model_node->first_attribute("multiples");

    // 不可缺省的元素
    if (!att_id || !att_name || !att_display_name)
      return false;

    ModelInfo model;
    model.id = att_id->value();
    model.name = att_name->value();
    model.display_name = att_display_name->value();


    if (att_dll)
      model.dll_name = att_dll->value();
    if (att_geo)
      model.has_geo_info = std::string(att_geo->value()) == "true";
    else  // 默认为真
      model.has_geo_info = true;
    if (att_category)
      model.category = att_category->value();
    else // 默认为原子实体
      model.category = "ATOMIC_ENTITY";
    if (att_multiples)
      model.multiples = att_multiples->value();
    else // 默认为1
      model.multiples = "1";

    auto params_node = model_node->first_node("Parameters");
    if (!params_node)
      return false;

    for (auto it = params_node->first_node("Parameter"); it;
         it = it->next_sibling()) {
      auto att_name = it->first_attribute("name");
      auto att_type = it->first_attribute("type");
      auto att_dn = it->first_attribute("displayName");
      auto att_usage = it->first_attribute("usage");
      auto att_value = it->first_attribute("value");
      auto att_unit = it->first_attribute("unit");

      if (!att_name || !att_type || !att_dn || !att_usage)
        return false;

      ModelParamItem item;
      item.name = att_name->value();
      item.type = att_type->value();
      item.display_name = att_dn->value();
      item.usage = att_usage->value();
      if (att_value)	// 允许不设置默认参数值
        item.value = att_value->value();
      if (att_unit)
        item.unit = att_unit->value();

      model.params[item.name] = std::move(item);
    }

    // 在上述过程均无异常的情况下最后进行拷贝
    info.id = model.id;
    info.name = model.name;
    info.display_name = model.display_name;
    info.dll_name = model.dll_name;
    info.category = model.category;
    info.has_geo_info = model.has_geo_info;
    info.multiples = model.multiples;
    info.params = model.params;

  } catch (...) {
    return false;
  }

  return true;
}

bool CSModelInfo::WriteXmlDoc(xml_document<char>& doc) {
  // 添加声明
  xml_node<> *decl_node = doc.allocate_node(node_declaration);
  decl_node->append_attribute(doc.allocate_attribute("version", "1.0"));
  decl_node->append_attribute(doc.allocate_attribute("encoding", "UTF-8"));
  doc.append_node(decl_node);

  // 添加根节点
  auto root = doc.allocate_node(node_element, "ModelInfo");
  doc.append_node(root);
  root->append_attribute(doc.allocate_attribute("id",
                         doc.allocate_string(info.id.c_str())));
  root->append_attribute(doc.allocate_attribute("name",
                         doc.allocate_string(info.name.c_str())));
  root->append_attribute(doc.allocate_attribute("displayName",
                         doc.allocate_string(info.display_name.c_str())));
  root->append_attribute(doc.allocate_attribute("dllName",
                         doc.allocate_string(info.dll_name.c_str())));
  root->append_attribute(doc.allocate_attribute("hasGeoInfo",
                         doc.allocate_string(info.has_geo_info ? "true" : "false")));
  root->append_attribute(doc.allocate_attribute("category",
                         doc.allocate_string(info.category.c_str())));
  root->append_attribute(doc.allocate_attribute("multiples",
                         doc.allocate_string(info.multiples.c_str())));

  auto model_parameters = doc.allocate_node(node_element, "Parameters");
  for (const auto& [_, params] : info.params) {
    auto model_parameter = doc.allocate_node(node_element, "Parameter");
    model_parameter->append_attribute(doc.allocate_attribute("name",
                                      doc.allocate_string(params.name.c_str())));
    model_parameter->append_attribute(doc.allocate_attribute("type",
                                      doc.allocate_string(params.type.c_str())));
    model_parameter->append_attribute(doc.allocate_attribute("displayName",
                                      doc.allocate_string(params.display_name.c_str())));
    model_parameter->append_attribute(doc.allocate_attribute("usage",
                                      doc.allocate_string(params.usage.c_str())));
    model_parameter->append_attribute(doc.allocate_attribute("value",
                                      doc.allocate_string(params.value.c_str())));
    model_parameter->append_attribute(doc.allocate_attribute("unit",
                                      doc.allocate_string(params.unit.c_str())));
    model_parameters->append_node(model_parameter);
  }
  root->append_node(model_parameters);
  return true;
}
