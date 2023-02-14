#pragma once

#include <string>
#include <type_traits>
#include "src/extern/pfr.hpp"
#include "src/extern/rapidxml-1.13/rapidxml.hpp"
#include "../tools/datastructure.hpp"

namespace carphymodel::component{
    
// #if __cplusplus >= 202002L

// template<typename T>
// concept Component = 
//     std::is_same_v<T, int> ||
//     std::is_same_v<T, float> ||
//     std::is_same_v<T, double> ||
//     std::is_enum_v<T> || 
//     requires{
//         std::string(T::token_list[0]);
//         pfr::get<0>(T{});
//     };

// #define Restrict Component

// #else

// #define Restrict typename

// #endif

template<typename RETURN_TYPE> inline RETURN_TYPE componentDeserialize(rapidxml::xml_node<char>* node);

template<typename T, size_t Index>
inline void emplace(T& tar, rapidxml::xml_node<char>* node) {
    if constexpr (Index < pfr::tuple_size_v<T>) {
        if constexpr (T::token_list[Index][0] == '!'){
            return;
        }else{
            auto tmp = node->first_node(T::token_list[Index]);
            if (tmp != 0) {
                pfr::get<Index>(tar) = componentDeserialize<std::remove_reference_t<decltype(pfr::get<Index>(tar))>>(tmp);
            }
            else {
                pfr::get<Index>(tar) = {};
            }
            emplace<T, Index + 1>(tar, node);
        }
    }
}

template<typename RETURN_TYPE>
inline RETURN_TYPE componentDeserialize(rapidxml::xml_node<char>* node){
    if constexpr (std::is_enum_v<RETURN_TYPE>){
        return (RETURN_TYPE)componentDeserialize<int>(node);
    }else{
        RETURN_TYPE tmp;
        emplace<RETURN_TYPE, 0>(tmp, node);
        return tmp;
    }
}

template<> inline double componentDeserialize<double>(rapidxml::xml_node<char>* node){
    return std::stof(node->value());
}

template<> inline float componentDeserialize<float>(rapidxml::xml_node<char>* node){
    return std::stof(node->value());
}

template<> inline int componentDeserialize<int>(rapidxml::xml_node<char>* node){
    return std::stoi(node->value());
}

template<> inline std::string componentDeserialize<std::string>(rapidxml::xml_node<char>* node){
    return std::string(node->value());
}

template<> inline Vector3 componentDeserialize<Vector3>(rapidxml::xml_node<char>* node){
    if (node->first_node("x") != 0){
        return Vector3{
            std::stof(node->first_node("x")->value()),
            std::stof(node->first_node("y")->value()),
            std::stof(node->first_node("z")->value())};
    }else{
        Quaternion q(
            std::stof(node->first_node("roll")->value()),
            std::stof(node->first_node("pitch")->value()),
            std::stof(node->first_node("yaw")->value()));
        return (Vector3)q;
    }
}

template<> inline Coordinate componentDeserialize<Coordinate>(rapidxml::xml_node<char>* node){
    return Coordinate{
        componentDeserialize<Vector3>(node->first_node("position")),
        componentDeserialize<Vector3>(node->first_node("altitude"))
    };
}

// template<typename T> 
// inline std::vector<T> componentDeserialize<std::vector<T>>(rapidxml::xml_node<char>* node){
//     std::vector<T> tmp;
//     auto child = node->first_node(0);
//     while(child){
//         tmp.push_back(componentDeserialize<T>(child));
//         child = child->next_sibling(0);
//     }
//     return tmp;
// }

}