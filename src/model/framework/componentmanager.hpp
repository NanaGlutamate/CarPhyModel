#pragma once

//#include <boost/hana/>
#include "../tools/metatools.hpp"
#include <tuple>
#include <vector>
#include <type_traits>
#include <map>
#include <set>
#include <queue>
#include <optional>

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
    using NormalComponents = std::tuple<std::vector<std::pair<std::size_t, Normals>>...>;
    using SingletonComponents = std::tuple<std::optional<Singletons>...>;
private:
    // counter of instance of EntityList<>
    // used to give each instance of EntityList<> a token to access cached entity list
    inline static size_t entityGroupCounter = 0;
    bool lock;
    size_t entityCounter;
    NormalComponents normalComponents;
    SingletonComponents singletonComponents;
    struct EntityGroupCache{
        std::map<size_t, std::vector<size_t>> content;
    } entityGroupCache;
    // struct Locker{
    //     ComponentManager& cm
    //     Locker(ComponentManager& cm): cm(cm){
    //         assert(!cm.lock);
    //         cm.lock = true;
    //     }
    //     ~Locker(){
    //         cm.lock = false;
    //     }
    // };

    // private function to build entity id cache for specific entity group
    template<typename ...Ty>
    void buildCache(size_t entityGroupID){
        assert(!lock);
        std::vector<size_t> tmp;
        std::tuple<typename std::vector<std::pair<std::size_t, Ty>>::iterator...> iters = {get<type_list_index<Ty, Normals...>::value>(normalComponents).begin()...};
        std::tuple<typename std::vector<std::pair<std::size_t, Ty>>::iterator...> ends = {get<type_list_index<Ty, Normals...>::value>(normalComponents).end()...};
        for(size_t index = 1; index < entityCounter; ++index){
            // TODO:
            bool isEnd = false;
            bool isEntityFit = true;
            // for_each over each type in ...Ty
            int tmp[sizeof...(Ty)] = {[&](){
                if(!isEntityFit){
                    return 0;
                }
                constexpr size_t CURRENT_INDEX = type_list_index<Ty, Normals...>::value;
                auto& currentIterator = get<CURRENT_INDEX>(iters);
                while(currentIterator != get<CURRENT_INDEX>(ends) && currentIterator->first < index){
                    ++currentIterator;
                };
                if(currentIterator->first != index){
                    isEntityFit = false;
                }
                if(currentIterator == get<CURRENT_INDEX>(ends)){
                    isEntityFit = false;
                    isEnd = true;
                }
                return 0;
            }()...};
            if(isEntityFit){
                tmp.push_back(index);
            }
            if(isEnd){
                break;
            }
        }
        content.emplace(std::make_pair(entityGroupID, std::move(tmp)));
    }
public:
    ComponentManager(): lock(false), entityCounter(0), normalComponents(), singletonComponents(), entityGroupCache(){};
private:
    class Modifier{
        // friend class ComponentManager;
        ComponentManager& componentManager;
        Modifier(ComponentManager& componentManager): componentManager(componentManager){
            // TODO: multi-threading?
            assert(!componentManager.lock);
            componentManager.lock = true;
        }
    public:
        size_t newEntity(){
            return ++entityCounter;
        }
        template<typename ...Ty>
        void addNormalComponent(std::size_t ID, const Ty& ...components){
            static_assert((... && (type_list_contains_v<Ty, Normals...>)), "component manager doesnot contain normal component of that type");
            int tmp[sizeof...(Ty)] = {(
                get<type_list_index<Ty, Normals...>::value>(componentManager.normalComponents).push_back({ID, components}),
                0
            )...};
        };
        template<typename ...Ty>
        void addSingletonComponent(const Ty& ...components){
            static_assert((... && (type_list_contains_v<Ty, Singletons...>)), "component manager doesnot contain singleton component of that type");
            int tmp[sizeof...(Ty)] = {(
                get<type_list_index<Ty, Singletons...>::value>(componentManager.singletonComponents) = components,
                0
            )...};
        }
        template<typename ...Ty>
        void ereaseSingletonComponent(){
            static_assert((... && (type_list_contains_v<Ty, Singletons...>)), "component manager doesnot contain singleton component of that type");
            int tmp[sizeof...(Ty)] = {(
                get<type_list_index<Ty, Singletons...>::value>(componentManager.singletonComponents) = std::nullopt, 
                0
            )...};
        }
        ~Modifier(){
            int tmp[sizeof...(Normals)] = {(
                std::sort(
                    get<type_list_index<Ty, Normals...>::value>(componentManager.normalComponents).begin(),
                    get<type_list_index<Ty, Normals...>::value>(componentManager.normalComponents).end(),
                    [](auto& x, auto& y){return (get<0>(x) < get<0>(y));}
                ),
                0
            )...};
            entityGroupCache.content.clear();
            componentManager.lock = false;
        };
    };
public:
    // get a component-manager modifier to modify the components of entities;
    // must be destructed before visit component
    Modifier getModifier(){return Modifier(*this);};
private:
    // TODO: lock to disable modify during iteration?
    template <typename ...ComponentTypes>
    struct EntityList{
        inline static size_t entityGroupID = 0;
    public:
        ComponentManager* cm;
        EntityList(ComponentManager* cm):cm(cm){
            if(entityGroupID == 0){
                entityGroupID = ++ComponentManager::entityGroupCounter;
            }
            if(cm->entityGroupCache.content.find(entityGroupID) == cm->entityGroupCache.content.end()){
                // TODO: sort types to reduce times of cache building?
                cm->buildCache<ComponentTypes...>(entityGroupID);
            }
        };
        struct ComponentIterator{
            ComponentManager* cm;
            std::vector<size_t>::iterator entityGroupIDIterator;
            std::vector<size_t>::iterator entityGroupIDIteratorEnd;
            std::tuple<typename std::vector<std::pair<std::size_t, ComponentTypes>>::iterator...> iters;
            ComponentIterator(ComponentManager* cm, size_t entityGroupID): cm(cm){
                auto& cacheLine = cm->entityGroupCache.content[entityGroupID];
                entityGroupIDIterator = cacheLine.begin();
                entityGroupIDIteratorEnd = cacheLine.end();
                iters = {get<type_list_index<ComponentTypes, Normals...>::value>(normalComponents).begin()...};
                if(entityGroupIDIterator != entityGroupIDIteratorEnd){
                    // TODO:
                }
            };
            void operator++(){
                ++currentEntityIndexInBuffer;
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
public:
    template <typename ...ComponentTypes>
    decltype(auto) getNormalComponent(){
        assert(!lock);
        static_assert((... && type_list_contains_v<ComponentTypes, Normals...>),
            "component manager doesnot contain normal component of that type");
        return EntityList<ComponentTypes...>(this);
    };
    template <typename ...ComponentTypes>
    decltype(auto) getSingletonComponent(){
        static_assert((... && type_list_contains_v<ComponentTypes, Singletons...>),
            "component manager doesnot contain singleton component of that type");
        return std::tuple<std::optional<ComponentTypes>&...>{
            std::get<type_list_index<ComponentTypes, Singletons...>::value>(
                singletonComponents
            )...
        };
    };
};

};