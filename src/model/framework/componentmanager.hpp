#pragma once

//#include <boost/hana/>
#include "../tools/metatools.hpp"
#include <tuple>
#include <vector>
#include <type_traits>
#include <map>
#include <set>
#include <queue>

template <typename ...T>
struct SingletonComponent{};

template <typename ...T>
struct NormalComponent{};

template <typename Test, typename ...List>
struct type_list_index{
    constexpr static std::size_t value = 0;
};

template <typename Test, typename ListStart, typename ...List>
struct type_list_index<Test, ListStart, List...>{
    constexpr static std::size_t value = type_list_index<Test, List...>::value + 1;
};

template <typename Test, typename ...List>
struct type_list_index<Test, Test, List...>{
    constexpr static std::size_t value = 0;
};

template <typename Test, typename ...List>
inline constexpr bool type_list_contains_v = (type_list_index<Test, List...>::value != sizeof...(List));

template <std::size_t ...indexes>
struct runtime_list{
    constexpr static std::array<std::size_t, sizeof...(indexes)> value{indexes...};
};

template <std::size_t ...indexes, typename ...Args>
constexpr decltype(auto) gets(std::tuple<Args...>& t){return std::tuple<decltype(get<indexes>(t))&...>(get<indexes>(t)...);};





template <typename S, typename N>
class ComponentManager;

template <typename ...Singletons, typename ...Normals>
class ComponentManager<SingletonComponent<Singletons...>, NormalComponent<Normals...>>{
public:
    using Entities = std::set<std::size_t>;
    using NormalComponents = std::tuple<std::map<std::size_t, Normals>...>;
    using SingletonComponents = std::tuple<Singletons...>;
    using SingletonAviliable = std::array<bool, sizeof...(Singletons)>;
private:
    Entities e;
    std::priority_queue<std::size_t, std::vector<std::size_t>, std::greater<std::size_t>> recycled_e;
    NormalComponents nc;
    SingletonComponents sc;
    SingletonAviliable sa;
    size_t entity_count;
public:
    ComponentManager():e(), nc(), sc(), sa(), entity_count(1){
        sa.fill(false);
    };
    size_t newEntity(){
        if(!recycled_e.empty()){
            auto tmp = recycled_e.top();
            recycled_e.pop();
            return tmp;
        }else return entity_count++;
    };
    template <typename ...Ty>
    void addNormalComponent(std::size_t ID, const Ty& ...components){
        static_assert((... && (type_list_contains_v<Ty, Normals...>)), "component manager donot contains normal component of that type");
        int tmp[sizeof...(Ty)] = {(
            get<type_list_index<Ty, Normals...>::value>(nc).emplace(ID, components),
            0
        )...};
        e.emplace(ID);
    };
    template <typename ...Ty>
    void addSingletonComponent(const Ty& ...components){
        static_assert((... && (type_list_contains_v<Ty, Singletons...>)), "component manager donot contains singleton component of that type");
        int tmp[sizeof...(Ty)] = {(
            get<type_list_index<Ty, Singletons...>::value>(sc) = components, 
            sa[type_list_index<Ty, Singletons...>::value] = true, 
            0
        )...};
    }
    template <typename ...Ty>
    void ereaseNormalComponent(std::size_t ID){
        static_assert((... && (type_list_contains_v<Ty, Normals...>)), "component manager donot contains normal component of that type");
        int tmp[sizeof...(Ty)] = {(
            get<type_list_index<Ty, Normals...>::value>(nc).erase(ID),
            0
        )...};
        if((... && (get<type_list_index<Normals, Normals...>::value>(nc).find(ID) == get<type_list_index<Normals, Normals...>::value>(nc).end()))){
            recycled_e.emplace(ID);
            e.erase(ID);
        }
    };
    template <typename ...Ty>
    void ereaseSingletonComponent(){
        static_assert((... && (type_list_contains_v<Ty, Singletons...>)), "component manager donot contains singleton component of that type");
        int tmp[sizeof...(Ty)] = {(
            sa[type_list_index<Ty, Singletons...>::value] = false, 
            0
        )...};
    }
    template <typename ...ComponentTypes>
    struct EntityList{
        EntityList(ComponentManager* cm):cm(cm){};
        ComponentManager* cm;
        struct ComponentIterator{
            ComponentManager* cm;
            Entities::iterator current_entity;
            bool aviliable(){
                auto current_entity_ID = *current_entity;
                return ((std::get<type_list_index<ComponentTypes, Normals...>::value>(cm->nc).find(current_entity_ID)
                        !=std::get<type_list_index<ComponentTypes, Normals...>::value>(cm->nc).end()) && ...);
            };
            void operator++(){
                do{
                    ++current_entity;
                }while(current_entity!=cm->e.end()&&!aviliable());
            };
            decltype(auto) operator*(){
                auto current_entity_ID = *current_entity;
                return std::tuple<const std::size_t, ComponentTypes&...>(
                    current_entity_ID,
                    std::get<type_list_index<ComponentTypes, Normals...>::value>(cm->nc).find(current_entity_ID)->second...
                );
            };
            bool operator!=(const ComponentIterator& other){
                return current_entity != other.current_entity || cm != other.cm;
            };
        };
        ComponentIterator begin(){
            ComponentIterator tmp{cm, cm->e.begin()};
            if(!cm->e.empty()&&!tmp.aviliable())++tmp;
            return tmp;
        };
        ComponentIterator end(){
            return ComponentIterator{cm, cm->e.end()};
        };
    };
    template <typename ...ComponentTypes>
    decltype(auto) getNormalComponent(){
        static_assert((... && type_list_contains_v<ComponentTypes, Normals...>),
            "component manager donot contains normal component of that type");
        return EntityList<ComponentTypes...>(this);
    };
    template <typename ...ComponentTypes>
    bool hasSingletonComponent(){
        static_assert((... && type_list_contains_v<ComponentTypes, Singletons...>),
            "component manager donot contains singleton component of that type");
        return (sa[type_list_index<ComponentTypes, Singletons...>::value] && ...);
    };
    template <typename ...ComponentTypes>
    decltype(auto) getSingletonComponent(){
        static_assert((... && type_list_contains_v<ComponentTypes, Singletons...>),
            "component manager donot contains singleton component of that type");
        assert(hasSingletonComponent<ComponentTypes...>());
        return std::tuple<ComponentTypes&...>{std::get<type_list_index<ComponentTypes, Singletons...>::value>(sc)...};
    };
};