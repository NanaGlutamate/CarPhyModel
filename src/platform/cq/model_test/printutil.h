#pragma once

#include <any>
#include <sstream>

#include "../csmodel_base/model_struct.h"
#include "../csconfig/type_define_info.h"

// 本文件定义复杂数据结构的参数打印方法

// 打印负责类型参数数值(数组)
std::string PrintArrayTypeParam(const std::string&, std::any&,
                                const config::TypeDefineInfo&);

// 打印复杂类型参数数值(结构体)
std::string PrintStructTypeParam(const std::string&, std::any&,
                                 const config::TypeDefineInfo&);

// 判断是否为基本类型
bool IsBasicType(const std::string& platfrom_type_) {
  return ("bool" == platfrom_type_ || "int8" == platfrom_type_ ||
          "uint8" == platfrom_type_ || "int16" == platfrom_type_ ||
          "uint16" == platfrom_type_ || "int32" == platfrom_type_ ||
          "uint32" == platfrom_type_ || "int64" == platfrom_type_ ||
          "uint64" == platfrom_type_ || "float32" == platfrom_type_ ||
          "float64" == platfrom_type_ || "float128" == platfrom_type_ ||
          "string" == platfrom_type_);
}

// 判断是否为数组类型
bool IsArrayType(const std::string& platfrom_type_) {
  return std::string::npos != platfrom_type_.find("[]");
}

// 获取数组元素数据类型
std::string GetArrayElementType(const std::string& platfrom_type_) {
  return platfrom_type_.substr(0, platfrom_type_.find("[]"));
}

// 平台数据类型转为C++数据类型
std::string ConvertPlatformTypeToCppType(const std::string platfrom_type_) {
  if ("bool" == platfrom_type_) {
    return "bool";
  } else if ("int8" == platfrom_type_) {
    return "int8_t";
  } else if ("uint8" == platfrom_type_) {
    return "uint8_t";
  } else if ("int16" == platfrom_type_) {
    return "int16_t";
  } else if ("uint16" == platfrom_type_) {
    return "uint16_t";
  } else if ("int32" == platfrom_type_) {
    return "int32_t";
  } else if ("uint32" == platfrom_type_) {
    return "uint32_t";
  } else if ("int64" == platfrom_type_) {
    return "int64_t";
  } else if ("uint64" == platfrom_type_) {
    return "uint64_t";
  } else if ("float32" == platfrom_type_) {
    return "float";
  } else if ("float64" == platfrom_type_) {
    return "double";
  } else if ("float128" == platfrom_type_) {
    return "long double";
  } else if ("string" == platfrom_type_) {
    return "std::string";
  }
  return platfrom_type_;
}

// 打印基本类型参数数值
std::string PrintBasicTypeParam(const std::string& param_cpp_type_,
                                std::any& var_value_) {
  if ("bool" == param_cpp_type_) {
    auto t = std::any_cast<bool>(var_value_);
    return (t) ? "true" : "false";
  } else if ("int8_t" == param_cpp_type_) {
    auto t = std::any_cast<int8_t>(var_value_);
    return std::to_string(t);
  } else if ("uint8_t" == param_cpp_type_) {
    auto t = std::any_cast<uint8_t>(var_value_);
    return std::to_string(t);
  } else if ("int16_t" == param_cpp_type_) {
    auto t = std::any_cast<int16_t>(var_value_);
    return std::to_string(t);
  } else if ("uint16_t" == param_cpp_type_) {
    auto t = std::any_cast<uint16_t>(var_value_);
    return std::to_string(t);
  } else if ("int32_t" == param_cpp_type_) {
    auto t = std::any_cast<int32_t>(var_value_);
    return std::to_string(t);
  } else if ("uint32_t" == param_cpp_type_) {
    auto t = std::any_cast<uint32_t>(var_value_);
    return std::to_string(t);
  } else if ("int64_t" == param_cpp_type_) {
    auto t = std::any_cast<int64_t>(var_value_);
    return std::to_string(t);
  } else if ("uint64_t" == param_cpp_type_) {
    auto t = std::any_cast<uint64_t>(var_value_);
    return std::to_string(t);
  } else if ("float" == param_cpp_type_) {
    auto t = std::any_cast<float>(var_value_);
    return std::to_string(t);
  } else if ("double" == param_cpp_type_) {
    auto t = std::any_cast<double>(var_value_);
    return std::to_string(t);
  } else if ("long double" == param_cpp_type_) {
    auto t = std::any_cast<long double>(var_value_);
    return std::to_string(t);
  }
  return std::any_cast<std::string>(var_value_);
}

// 打印负责类型参数数值(数组)
std::string PrintArrayTypeParam(const std::string& array_type_,
                                std::any& var_value_,
                                const config::TypeDefineInfo& types_info_) {
  auto va = std::any_cast<std::vector<std::any>>(var_value_);

  std::stringstream ss;

  auto element_platform_type_ = GetArrayElementType(array_type_);
  // 数组元素为基本类型
  if (IsBasicType(element_platform_type_)) {
    ss << "[";
    std::string element_cpp_type_ =
        ConvertPlatformTypeToCppType(element_platform_type_);
    for (auto pa : va) {
      ss << PrintBasicTypeParam(element_cpp_type_, pa) << ",";
    }
    ss << "]";
    return ss.str();
  }

  // 数组元素为结构体类型
  if (auto it = types_info_.type_defines.find(element_platform_type_);
      it != types_info_.type_defines.end()) {
    ss << "[";
    for (auto pa : va) {
      ss << PrintStructTypeParam(element_platform_type_, pa, types_info_)
         << ",";
    }
    ss << "]";
    return ss.str();
  }

  ss << "typedefine.xml文件中查找不到数据类型的定义: "
     << element_platform_type_ << std::endl;
  return ss.str();
}

// 打印复杂类型参数数值(结构体)
std::string PrintStructTypeParam(const std::string& struct_type_,
                                 std::any& var_value_,
                                 const config::TypeDefineInfo& types_info_) {
  std::stringstream ss;

  config::TypeDefine type_;
  if (auto it = types_info_.type_defines.find(struct_type_);
      it != types_info_.type_defines.end()) {
    type_ = it->second;
  } else {
    ss << "typedefine.xml文件中查找不到数据类型的定义: " << struct_type_
       << std::endl;
    return ss.str();
  }

  ss << "{";
  auto vm_ = std::any_cast<CSValueMap>(var_value_);
  for (auto& vit : vm_) {
    std::string member_platform_type_;
    if (auto mit = type_.params.find(vit.first); mit != type_.params.end()) {
      member_platform_type_ = mit->second;
    }

    ss << vit.first << ":";
    if (IsBasicType(member_platform_type_)) {
      // 参数为基本类型
      std::string cpp_type_ =
          ConvertPlatformTypeToCppType(member_platform_type_);
      ss << PrintBasicTypeParam(cpp_type_, vit.second);
    } else if (IsArrayType(member_platform_type_)) {
      // 参数为数组类型
      ss << PrintArrayTypeParam(member_platform_type_, vit.second, types_info_);
    } else {
      // 参数为结构体类型
      ss << PrintStructTypeParam(member_platform_type_, vit.second,
                                 types_info_);
    }
    ss << ",";
  }
  ss << "}";
  return ss.str();
}

// 打印参数数值
std::string Print(const std::string& platform_type_, std::any& param_value_,
                  const config::TypeDefineInfo& types_info_) {
  std::stringstream ss;

  // 参数为基本类型
  if (IsBasicType(platform_type_)) {
    // 平台数据类型转为C++类型
    std::string cpp_type_ = ConvertPlatformTypeToCppType(platform_type_);
    ss << PrintBasicTypeParam(cpp_type_, param_value_);
    return ss.str();
  }

  // 参数为数组类型
  if (IsArrayType(platform_type_)) {
    ss << PrintArrayTypeParam(platform_type_, param_value_, types_info_);
    return ss.str();
  }

  // 参数为结构体类型
  ss << PrintStructTypeParam(platform_type_, param_value_, types_info_);
  return ss.str();
}
