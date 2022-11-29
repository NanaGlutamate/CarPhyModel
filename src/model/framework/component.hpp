#pragma once

#include <string>
#include <type_traits>
#include "src/extern/pfr.hpp"
#include "src/extern/rapidxml-1.13/rapidxml.hpp"

namespace carPhyModel::components{

template<typename RETURN_TYPE> inline RETURN_TYPE componentDeserialize(rapidxml::xml_node<char>* node);

template<typename T, size_t Index>
inline void emplace(T& tar, rapidxml::xml_node<char>* node) {
    if constexpr (Index < pfr::tuple_size_v<T>) {
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

template<typename RETURN_TYPE>
inline RETURN_TYPE componentDeserialize(rapidxml::xml_node<char>* node){
    RETURN_TYPE tmp;
    emplace<RETURN_TYPE, 0>(tmp, node);
    return tmp;
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

}