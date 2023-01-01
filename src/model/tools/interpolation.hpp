#pragma once

#include <array>

namespace carPhyModel{

template<typename Index, typename Element, size_t Dimension>
struct Data{
    std::array<Index, Dimension> data;
};

template<typename Value, typename InterFunc>
decltype(auto) getInterpolationFunction(){return [=](){};}

}