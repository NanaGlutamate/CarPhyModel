#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <array>
#include <type_traits>
#include <algorithm>
#include <map>
#include <optional>
#include "../tools/metatools.hpp"
#include "../tools/myassert.hpp"

namespace carphymodel{

// #if __cplusplus >= 202002L

// template<typename T>
// concept ComponentType = std::is_trivial_v<T>;

// // decorator for singleton component and normal component
// template <ComponentType ...T>
// struct SingletonComponent{};

// template <ComponentType ...T>
// struct NormalComponent{};

// #else

// decorator for singleton component and normal component
template <typename ...T>
struct SingletonComponent{};

template <typename ...T>
struct NormalComponent{};

// #endif

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
constexpr decltype(auto) gets(std::tuple<Args...>& t){return std::tuple<decltype(std::get<indexes>(t))&...>(get<indexes>(t)...);};





template <typename S, typename N>
class ComponentManager;

template <typename ...Singletons, typename ...Normals>
class ComponentManager<SingletonComponent<Singletons...>, NormalComponent<Normals...>>{
public:
    using SingletonComponentTypes = SingletonComponent<Singletons...>;
    using NormalComponentTypes = NormalComponent<Normals...>;

    ComponentManager(): lock(false), entityCounter(0), normalComponents(), singletonComponents(), entityGroupCache()/*, entityComponentCache()*/{};

    class Modifier{
    public:
        operator bool() const { return componentManager.lock; }
        size_t newEntity(){
            return ++componentManager.entityCounter;
        }
        template<typename ...Ty>
        void addNormalComponents(std::size_t ID, const Ty& ...components){
            static_assert((... && (type_list_contains_v<Ty, Normals...>)), "component manager doesnot contain normal component of that type");
            int tmp[sizeof...(Ty)] = {(
                std::get<type_list_index<Ty, Normals...>::value>(componentManager.normalComponents).push_back({ID, components}),
                0
            )...};
            if (componentManager.entityCounter < ID) { componentManager.entityCounter = ID; }
        };
        // TODO: eraseNormalComponent
        template<typename ...Ty>
        void addSingletonComponents(const Ty& ...components){
            static_assert((... && (type_list_contains_v<Ty, Singletons...>)), "component manager doesnot contain singleton component of that type");
            int tmp[sizeof...(Ty)] = {(
                std::get<type_list_index<Ty, Singletons...>::value>(componentManager.singletonComponents) = components,
                0
            )...};
        }
        template<typename ...Ty>
        void ereaseSingletonComponents(){
            static_assert((... && (type_list_contains_v<Ty, Singletons...>)), "component manager doesnot contain singleton component of that type");
            int tmp[sizeof...(Ty)] = {(
                std::get<type_list_index<Ty, Singletons...>::value>(componentManager.singletonComponents).reset(), 
                0
            )...};
        }
        ~Modifier(){
            int tmp1[sizeof...(Normals)] = {(
                std::sort(
                    std::get<type_list_index<Normals, Normals...>::value>(componentManager.normalComponents).begin(),
                    std::get<type_list_index<Normals, Normals...>::value>(componentManager.normalComponents).end(),
                    [](auto& x, auto& y){return (std::get<0>(x) < std::get<0>(y));}
                ),
                0
            )...};
            // TODO: remove cache?
            // int tmp2[sizeof...(Normals)] = {[&](){
            //     constexpr size_t CURRENT_TYPE = type_list_index<Normals, Normals...>::value;
            //     componentManager.entityComponentCache[CURRENT_TYPE].clear();
            //     auto& v = std::get<CURRENT_TYPE>(componentManager.normalComponents);
            //     for(size_t i = 0; i < v.size(); ++i){
            //         componentManager.entityComponentCache[CURRENT_TYPE].emplace(v[i].first, i);
            //     }
            //     return 0;
            // }()...};
            componentManager.entityGroupCache.clear();
            componentManager.lock = false;
        };
    private:
        friend class ComponentManager;
        ComponentManager& componentManager;
        Modifier(ComponentManager& componentManager): componentManager(componentManager){
            // TODO: multi-threading(mutex)?
            my_assert(!componentManager.lock, "Component Manager Locked");
            componentManager.lock = true;
        }
    };

    // get a component-manager modifier to modify the components of entities;
    // must be destructed before visit component
    Modifier getModifier(){return Modifier(*this);};

    template <typename ...ComponentTypes>
    decltype(auto) getNormal(){
        // TODO: multithreading?
        my_assert(!lock, "Component Manager Locked");
        static_assert((... && type_list_contains_v<ComponentTypes, Normals...>),
            "component manager doesnot contain normal component of that type");
        return EntityList<ComponentTypes...>(this);
    };
    
    template <typename ComponentType>
    std::optional<std::reference_wrapper<ComponentType>> getSpecificNormal(size_t ID){
        my_assert(!lock, "Component Manager Locked");
        static_assert(type_list_contains_v<ComponentType, Normals...>,
            "component manager doesnot contain normal component of that type");
        // auto& cache = entityComponentCache[type_list_index<ComponentType, Normals...>::value];
        // if(auto it = cache.find(ID); it != cache.end()){return std::get<std::vector<std::pair<size_t, ComponentType>>>(normalComponents)[it->second].second;}
        // else{return std::nullopt;}
        using TargetType = std::vector<std::pair<size_t, ComponentType>>;
        auto& specific = std::get<TargetType>(normalComponents);
        if(auto it = std::find_if(specific.begin(), specific.end(), [ID](auto p){return p.first==ID;}); it!=specific.end()){
            return it->second;
        }else{return std::nullopt;}
    };
    
    template <typename ...ComponentTypes>
    decltype(auto) getSingleton(){
        static_assert((... && type_list_contains_v<ComponentTypes, Singletons...>),
            "component manager doesnot contain singleton component of that type");
        return std::tuple<std::optional<ComponentTypes>&...>{
            std::get<type_list_index<ComponentTypes, Singletons...>::value>(
                singletonComponents
            )...
        };
    };
    
    template <typename ComponentType>
    decltype(auto) getSpecificSingleton(){
        static_assert(type_list_contains_v<ComponentType, Singletons...>,
            "component manager doesnot contain singleton component of that type");
        return std::get<type_list_index<ComponentType, Singletons...>::value>(singletonComponents);
    }
    
    void clear(){
        my_assert(!lock, "Component Manager Locked");
        entityCounter = 0;
        normalComponents.clear();
        int tmp[sizeof...(Singletons)]={(
            std::get<type_list_index<Singletons, Singletons...>::value>(
                singletonComponents
            ).reset(),
            0
        )...};
    }

private:

    using NormalComponents = std::tuple<std::vector<std::pair<std::size_t, Normals>>...>;
    using SingletonComponents = std::tuple<std::optional<Singletons>...>;
    // counter of instance of EntityList<>
    // used to give each instance of EntityList<> a token to access cached entity list
    inline static size_t entityGroupCounter = 0;
    bool lock;
    size_t entityCounter;
    NormalComponents normalComponents;
    SingletonComponents singletonComponents;
    // entity group ID -> list of entity id which fits this group
    std::map<size_t, std::vector<size_t>> entityGroupCache;
    // ComponentType -> entity ID -> index of component belonging to this entity
    // std::array<std::map<size_t, size_t>, sizeof...(Normals)> entityComponentCache;

    // private function to build entity id cache for specific entity group
    template<typename ...Ty>
    void buildCache(size_t entityGroupID){
        my_assert(!lock, "Component Manager Locked");
        static_assert((... && type_list_contains_v<Ty, Normals...>),
            "component manager doesnot contain normal component of that type");
        std::vector<size_t> cacheLine;
        std::tuple<typename std::vector<std::pair<std::size_t, Ty>>::iterator...> iters = {
            std::get<type_list_index<Ty, Normals...>::value>(normalComponents).begin()...
        };
        std::tuple<typename std::vector<std::pair<std::size_t, Ty>>::iterator...> ends = {
            std::get<type_list_index<Ty, Normals...>::value>(normalComponents).end()...
        };
        for(size_t index = 1; index <= entityCounter; ++index){
            bool isEnd = false;
            bool isEntityFit = true;
            // for_each over each type in ...Ty
            int tmp[sizeof...(Ty)] = {[&](){
                using ThisType = typename std::vector<std::pair<std::size_t, Ty>>::iterator;
                // TODO: why compile error while using CURRENT_TYPE as template parameter?
                constexpr size_t CURRENT_TYPE = type_list_index<Ty, Normals...>::value;
                auto& currentIterator = std::get<ThisType>(iters);
                while(currentIterator != std::get<ThisType>(ends) && currentIterator->first < index){++currentIterator;}
                if (currentIterator == std::get<ThisType>(ends)) {
                    isEntityFit = false;
                    isEnd = true;
                }else if(currentIterator->first != index){isEntityFit = false;}
                return 0;
            }()...};
            if(isEntityFit){cacheLine.push_back(index);}
            if(isEnd){break;}
        }
        entityGroupCache.emplace(entityGroupID, std::move(cacheLine));
    }

    // TODO: lock to disable modify during iteration?
    template <typename ...ComponentTypes>
    struct EntityList{
    public:
        inline static size_t entityGroupID = 0;
        ComponentManager* cm;
        EntityList(ComponentManager* cm):cm(cm){
            if(entityGroupID == 0){
                entityGroupID = ++ComponentManager::entityGroupCounter;
            }
            if(cm->entityGroupCache.find(entityGroupID) == cm->entityGroupCache.end()){
                // TODO: sort types to reduce times of cache building?
                cm->buildCache<ComponentTypes...>(entityGroupID);
            }
        };
        // TODO: template<typename ...Ty> decltype(auto) monitor();
        // returns iterator of tuple<ComponentTypes..., optional<Ty>...>
        struct ComponentIterator{
            struct EndIterator{
                constexpr EndIterator() = default;
            };
            ComponentManager* cm;
            std::vector<size_t>::iterator entityIDIterator, entityIDIteratorEnd;
            std::tuple<typename std::vector<std::pair<std::size_t, ComponentTypes>>::iterator...> iters;
            ComponentIterator(ComponentManager* cm): cm(cm){
                auto& cacheLine = cm->entityGroupCache[entityGroupID];
                entityIDIterator = cacheLine.begin();
                entityIDIteratorEnd = cacheLine.end();
                if(entityIDIterator != entityIDIteratorEnd){
                    iters = {std::get<type_list_index<ComponentTypes, Normals...>::value>(cm->normalComponents).begin()...};
                    auto entity = *entityIDIterator;
                    int tmp[sizeof...(ComponentTypes)] = {[&](){
                        // CAUTION: may not unique through each lambda
                        constexpr size_t CURRENT_TYPE = type_list_index<ComponentTypes, ComponentTypes...>::value;
                        while(std::get<CURRENT_TYPE>(iters)->first != entity){++std::get<CURRENT_TYPE>(iters);}
                        return 0;
                    }()...};
                }
            };
            void operator++(){
                ++entityIDIterator;
                if (entityIDIterator == entityIDIteratorEnd) { return; }
                auto entity = *entityIDIterator;
                int tmp[sizeof...(ComponentTypes)] = {[&](){
                    // CAUTION: may not unique through each lambda
                    constexpr size_t CURRENT_TYPE = type_list_index<ComponentTypes, ComponentTypes...>::value;
                    do{++std::get<CURRENT_TYPE>(iters);}while(std::get<CURRENT_TYPE>(iters)->first != entity);
                    return 0;
                }()...};
            };
            decltype(auto) operator*(){
                return std::tuple<const std::size_t&, ComponentTypes&...>(
                    *entityIDIterator,
                    std::get<type_list_index<ComponentTypes, ComponentTypes...>::value>(iters)->second...
                );
            };
            bool operator!=(const EndIterator other){
                return entityIDIterator != entityIDIteratorEnd;
            };
        };
        ComponentIterator begin(){
            return ComponentIterator(cm);
        };
        constexpr typename ComponentIterator::EndIterator end(){
            typename ComponentIterator::EndIterator tmp;
            return tmp;
        };
    };
};

};