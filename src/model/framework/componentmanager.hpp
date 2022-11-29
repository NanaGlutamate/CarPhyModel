#pragma once

//#include <boost/hana/>
#include "../tools/metatools.hpp"
#include <tuple>
#include <vector>
#include <type_traits>
#include <map>
#include <set>
#include <queue>

namespace carPhyModel{

// decorator for singleton component and normal component
template <typename ...T>
struct SingletonComponent{};

template <typename ...T>
struct NormalComponent{};

// indexer for type list
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
    // TODO: use vector instead
    using NormalComponents = std::tuple<std::map<std::size_t, Normals>...>;
    using SingletonComponents = std::tuple<Singletons...>;
    using SingletonComponentsState = std::array<bool, sizeof...(Singletons)>;
private:
    Entities entities;
    std::priority_queue<std::size_t, std::vector<std::size_t>, std::greater<std::size_t>> recycled_e;
    NormalComponents normalComponents;
    SingletonComponents singletonComponents;
    SingletonComponentsState singletonComponentsState;
    size_t entity_count;
private:
    class Modifier{
        ComponentManager& componentManager;
        Modifier(ComponentManager& componentManager): componentManager(componentManager){};
    public:
        
        ~Modifier(){};
    };
public:
    // get a component-manager modifier to modify the components;
    // must be destructed before visit component
    Modifier getModifier(){return Modifier(*this);};
    ComponentManager(): entities(), normalComponents(), singletonComponents(), singletonComponentsState(), entity_count(1){
        singletonComponentsState.fill(false);
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
            get<type_list_index<Ty, Normals...>::value>(normalComponents).emplace(ID, components),
            0
        )...};
        entities.emplace(ID);
    };
    template <typename ...Ty>
    void addSingletonComponent(const Ty& ...components){
        static_assert((... && (type_list_contains_v<Ty, Singletons...>)), "component manager donot contains singleton component of that type");
        int tmp[sizeof...(Ty)] = {(
            get<type_list_index<Ty, Singletons...>::value>(singletonComponents) = components, 
            singletonComponentsState[type_list_index<Ty, Singletons...>::value] = true, 
            0
        )...};
    }
    template <typename ...Ty>
    void ereaseNormalComponent(std::size_t ID){
        static_assert((... && (type_list_contains_v<Ty, Normals...>)), "component manager donot contains normal component of that type");
        int tmp[sizeof...(Ty)] = {(
            get<type_list_index<Ty, Normals...>::value>(normalComponents).erase(ID),
            0
        )...};
        if((... && (get<type_list_index<Normals, Normals...>::value>(normalComponents).find(ID) == get<type_list_index<Normals, Normals...>::value>(normalComponents).end()))){
            recycled_e.emplace(ID);
            entities.erase(ID);
        }
    };
    template <typename ...Ty>
    void ereaseSingletonComponent(){
        static_assert((... && (type_list_contains_v<Ty, Singletons...>)), "component manager donot contains singleton component of that type");
        int tmp[sizeof...(Ty)] = {(
            singletonComponentsState[type_list_index<Ty, Singletons...>::value] = false, 
            0
        )...};
    }
    template <typename ...ComponentTypes>
    struct EntityList{
    // private:
    //     static size_t ComponentManagerVersion = 0;
    //     static void indexCache;
    public:
        EntityList(ComponentManager* cm):cm(cm){};
        ComponentManager* cm;
        struct ComponentIterator{
            ComponentManager* cm;
            Entities::iterator current_entity;
            bool aviliable(){
                auto current_entity_ID = *current_entity;
                return ((std::get<type_list_index<ComponentTypes, Normals...>::value>(cm->normalComponents).find(current_entity_ID)
                        !=std::get<type_list_index<ComponentTypes, Normals...>::value>(cm->normalComponents).end()) && ...);
            };
            void operator++(){
                do{
                    ++current_entity;
                }while(current_entity!=cm->entities.end()&&!aviliable());
            };
            decltype(auto) operator*(){
                auto current_entity_ID = *current_entity;
                return std::tuple<const std::size_t, ComponentTypes&...>(
                    current_entity_ID,
                    std::get<type_list_index<ComponentTypes, Normals...>::value>(cm->normalComponents).find(current_entity_ID)->second...
                );
            };
            bool operator!=(const ComponentIterator& other){
                return current_entity != other.current_entity || cm != other.cm;
            };
        };
        ComponentIterator begin(){
            ComponentIterator tmp{cm, cm->entities.begin()};
            if(!cm->entities.empty()&&!tmp.aviliable())++tmp;
            return tmp;
        };
        // TODO: constexpr
        ComponentIterator end(){
            return ComponentIterator{cm, cm->entities.end()};
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
        return (singletonComponentsState[type_list_index<ComponentTypes, Singletons...>::value] && ...);
    };
    template <typename ...ComponentTypes>
    decltype(auto) getSingletonComponent(){
        static_assert((... && type_list_contains_v<ComponentTypes, Singletons...>),
            "component manager donot contains singleton component of that type");
        // assert(hasSingletonComponent<ComponentTypes...>());
        return std::tuple<ComponentTypes&...>{std::get<type_list_index<ComponentTypes, Singletons...>::value>(singletonComponents)...};
    };
};

};