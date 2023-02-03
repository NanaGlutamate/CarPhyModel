#pragma once

#include <array>

namespace carphymodel{

template<typename Con>
struct LinearCombinationView{
    Con& c1;
    Con& c2;
    double rate;
    decltype(auto) operator[](size_t i) const{return c1[i] * rate + c2[i] * (1 - rate);}
};

template<>
struct LinearCombinationView<double>{
    double c1;
    double c2;
    double rate;
    operator double(){return c1 * rate + c2 * (1 - rate);}
};

template<typename Con>
struct LinearCombination{
    Con c1;
    Con c2;
    double rate;
    decltype(auto) operator[](size_t i) const{return c1[i] * rate + c2[i] * (1 - rate);}
    LinearCombination(LinearCombination&& l): c1(std::move(l.c1)), c2(std::move(l.c2)), rate(rate){}
};

template<>
struct LinearCombination<double>{
    double c1;
    double c2;
    double rate;
    operator double(){return c1 * rate + c2 * (1 - rate);}
};

template<typename It1, typename It2, typename Val, typename Pred>
decltype(auto) linerInterpolate(It1 it1, It2 it2, Val val, Pred pred){
    if(it1 + 1 == it2) return LinearCombinationView<decltype(*it1)>{*it1, *it1, 1.};
    auto upper_it = std::lower_bound(it1, it2, val, [&](auto&& v, Val val){return pred(v) < val;});
    if(upper_it == it2){
        upper_it = it2 - 1;
    }else if(upper_it == it1){
        upper_it = it1 + 1;
    }
    It1 lower_it = upper_it - 1;
    return LinearCombinationView<decltype(*it1)>{
        *lower_it, 
        *upper_it, 
        (pred(*upper_it)-val)/(pred(*upper_it)-pred(*lower_it))
    };
}

}