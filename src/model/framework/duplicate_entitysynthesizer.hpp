#ifndef __SRC_MODEL_ENTITYSYNTHESIZER_HPP__
#define __SRC_MODEL_ENTITYSYNTHESIZER_HPP__

#include <tuple>
#include <vector>

// template <typename _Ty>
// struct is_const{constexpr static bool value=false;using origin_type = typename _Ty;};

// template <typename _Ty>
// struct is_const<const _Ty>{constexpr static bool value=true;using origin_type = typename _Ty;};

// template <typename _Ty>
// using origin_type = typename is_const<_Ty>::origin_type;

template <bool _Condition, typename _TrueType, typename _FalseType>
struct conditional_type{};

template <typename _TrueType, typename _FalseType>
struct conditional_type<true, _TrueType, _FalseType>{using type = _TrueType;};

template <typename _TrueType, typename _FalseType>
struct conditional_type<false, _TrueType, _FalseType>{using type = _FalseType;};

template <typename _Con, typename _Tar>
struct as_const{using type = typename conditional_type<std::is_const_v<_Con>, const _Tar, _Tar>::type;};

template <typename _Con, typename _Tar>
using as_const_t = typename as_const<_Con, _Tar>::type;

// 从多个vector<tuple<ID, component>>中取出第一个ID等于给定ID的component合并成tuple
template<typename _Index, typename... _Rest>
class IDMatcher;

template<typename _Index, typename _First, typename... _Rest>
class IDMatcher<_Index, _First, _Rest...>: private IDMatcher<_Index, _Rest...>{
public:
    using ID = _Index;
    using F = _First;
    using Fvec = std::vector<std::tuple<ID, F>>;
    using Self = IDMatcher<ID, F, _Rest...>;
    using Succ = IDMatcher<ID, _Rest...>;

    const Fvec& fvec;
    IDMatcher(const Fvec& fvec, const std::vector<std::tuple<ID, _Rest>>&... succ):fvec(fvec), Succ(succ...) {};
    std::tuple<const F&, const _Rest&...> find(ID id){
        auto tar = std::find_if(fvec.begin(), fvec.end(), [&](const std::tuple<ID, F>& t){
            return std::get<0>(t) == id;
        });
        auto ret = std::tuple_cat(std::tuple<const F&>(std::get<1>(*tar)), Succ::find(id));
        return ret;
    };
};

template<typename _Index>
class IDMatcher<_Index>{
public:
    using ID = _Index;
    using Self = IDMatcher<ID>;
    IDMatcher(){};
    std::tuple<> find(ID id){
        return std::make_tuple();
    };
};

// 遍历主键元素，与剩余vector中ID相同的组件配为tuple返回
template <typename _Index, typename _MainKey, typename... _Rest>
class EntitySynthesizer{
public:
    using ID = _Index;
    using M = _MainKey;
    using M_origin = std::remove_const_t<M>;
    using Mvec = typename conditional_type<std::is_const_v<M>, const std::vector<std::tuple<ID, M_origin>>, std::vector<std::tuple<ID, M_origin>>>::type;
    using Self = EntitySynthesizer<ID, _MainKey, _Rest...>;
    using R = std::tuple<ID, M&, const _Rest&...>;
private:
    IDMatcher<_Index, _Rest...> matcher;
    Mvec& mvec;
public:
    struct iter{
        ID id;
        Self * self;
        bool operator!=(const iter& other){
            return self != other.self || id != other.id;
        };
        iter& operator++(){
            id++;
            return *this;
        };
        iter operator++(int){
            iter ret = *this;
            id++;
            return ret;
        };
        R operator*(){
            return self->operator[](id);
        };
    };
    EntitySynthesizer(Mvec& mainKey, const std::vector<std::tuple<_Index, _Rest>>&... rest):matcher(rest...), mvec(mainKey){};
    iter begin(){
        return iter{0, this};
    };
    iter end(){
        return iter{size(), this};
    };
    size_t size(){
        return mvec.size();
    };
    R operator[](size_t id){
        ID cid=std::get<0>(mvec[id]);
        return std::tuple_cat(std::tuple<ID, M&>(cid, std::get<1>(mvec[id])), matcher.find(cid));
    };
};

// 将component组装成entity对应的tuple，其中只有第一个component可修改，避免system间数据竞争
template<typename _Index, typename _Key, typename... _Element>
decltype(auto) make_entity(std::vector<std::tuple<_Index, _Key>>& mainKey, const std::vector<std::tuple<_Index, _Element>>&... rest){
    return EntitySynthesizer<_Index, _Key, _Element...>(mainKey, rest...);
};

template<typename _Index, typename _Key, typename... _Element>
decltype(auto) make_entity(const std::vector<std::tuple<_Index, _Key>>& mainKey, const std::vector<std::tuple<_Index, _Element>>&... rest){
    return EntitySynthesizer<_Index, const _Key, _Element...>(mainKey, rest...);
};

#endif