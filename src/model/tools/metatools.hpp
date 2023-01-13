#pragma once

#include <array>

namespace carPhyModel::mymeta{

// 类型列表
template <typename ...T>
struct type_list{};

// 获取类型在类型列表中index
template <typename Test, typename ...List>
struct get_type_list_index{
    constexpr static inline std::size_t value = 0;
};

template <typename Test, typename ListStart, typename ...List>
struct get_type_list_index<Test, ListStart, List...>{
    constexpr static inline std::size_t value = get_type_list_index<Test, List...>::value + 1;
};

template <typename Test, typename ...List>
struct get_type_list_index<Test, Test, List...>{
    constexpr static inline std::size_t value = 0;
};

// 判断是否在列表
template <typename Test, typename ...List>
inline constexpr bool is_in_list_v = (get_type_list_index<Test, List...>::value != sizeof...(List));

// 静态列表
template <std::size_t ...indexes>
struct static_list {};

// 类型列表转index列表
template <typename Tar, typename LookUp>
struct to_index_static_list;

template <typename ...Tar, typename ...LookUp>
struct to_index_static_list<type_list<Tar...>, type_list<LookUp...>>{
    using type = static_list<get_type_list_index<Tar, LookUp...>::value...>;
};

// 静态列表转array
template <typename T>
struct to_array;

template <std::size_t ...indexes>
struct to_array<static_list<indexes...>>{
    constexpr static std::array<std::size_t, sizeof...(indexes)> value{indexes...};
};

// 拼接静态列表
template <typename L1, typename L2>
struct concatenate;

template <std::size_t ...L1Value, std::size_t ...L2Value>
struct concatenate<static_list<L1Value...>, static_list<L2Value...>>{
    using type = static_list<L1Value..., L2Value...>;
};

// 对类型的find_if

template <template <typename t> typename func, typename fallback, typename ...L>
struct find_if;

template <template <typename t> typename func, typename fallback, typename L1, typename ...L>
struct find_if<func, fallback, L1, L...>{
    using type = typename std::conditional_t<
        func<L1>::value,
        L1,
        typename find_if<func, L...>::type
    >;
};

template <template <typename t> typename func, typename fallback, typename L1>
struct find_if<func, fallback, L1>{
    using type = typename std::conditional_t<
        func<L1>::value,
        L1,
        fallback
    >;
};

// 函数式filter
template <template <std::size_t t> typename func, typename L>
struct filter;

template <template <std::size_t t> typename func, std::size_t first, std::size_t ...indexes>
struct filter<func, static_list<first, indexes...>>{
    using type = typename std::conditional_t<
            func<first>::value, 
            typename concatenate<static_list<first>, typename filter<func, static_list<indexes...>>::type>::type, 
            typename filter<func, static_list<indexes...>>::type
        >;
};

template <template <std::size_t t> typename func>
struct filter<func, static_list<>>{
    using type = static_list<>;
};

// 编译期比较模板
template <std::size_t tar>
struct template_greater{
    template <std::size_t t>
    struct evaluate{
        static constexpr bool value = (t>tar);
    };
};

template <std::size_t tar>
struct template_smaller{
    template <std::size_t t>
    struct evaluate{
        static constexpr bool value = (t<tar);
    };
};

// 去重快排
template <typename T>
struct deduplicate_sort;

template <std::size_t first, std::size_t ...indexes>
struct deduplicate_sort<static_list<first, indexes...>>{
    using type = typename concatenate<
            typename deduplicate_sort<
                typename filter<template_smaller<first>::template evaluate,
                    static_list<indexes...>
                >::type
            >::type,
            typename concatenate<
                static_list<first>,
                typename deduplicate_sort<
                    typename filter<template_greater<first>::template evaluate, 
                        static_list<indexes...>
                    >::type
                >::type
            >::type
        >::type;
};

template <>
struct deduplicate_sort<static_list<>>{
    using type = static_list<>;
};

// using t = typename deduplicate_sort<static_list<3, 1, 2, 5, 5, 2, 0>>::type;
// auto v = to_array<t>::value;

};