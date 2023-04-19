#include <algorithm>
#include "carbuilder.h"

#include "tools/metatools.hpp"
#include "framework/component.hpp"
#include "src/extern/rapidxml-1.13/rapidxml.hpp"
#include "tools/myassert.hpp"

#include "hull/carhull.h"
#include "protection/carprotection.h"
#include "damage/cardamage.h"
#include "sensors/carsensor.h"
#include "firecontrolsystem/carfcs.hpp"
#include "toolsystem/hitsystem.hpp"
#include "toolsystem/ballisticsystem.hpp"

namespace{

using namespace rapidxml;
using namespace std;
using namespace carphymodel;
using namespace carphymodel::component;
using namespace carphymodel::mymeta;

class CStyleString{
public:
    char*s;
    CStyleString(size_t n):s(new char[n]){};
    CStyleString(const CStyleString& s)=delete;
    void operator=(const CStyleString& s)=delete;
    CStyleString(const std::string& str):s(new char[str.size()+5]){memcpy(s,str.c_str(),str.size()+1);};
    ~CStyleString(){delete[] s;};
};

template<typename Ty>
struct loadComponent;

template<typename ...Ty>
struct loadComponent<NormalComponent<Ty...>>{
    void operator()(
        size_t ID, 
        rapidxml::xml_node<char>* component, 
        carphymodel::Components::Modifier& handle, 
        const std::vector<string_view>& token_list
    ){
        bool match = (... || [&](){
            if(component->name() == token_list[get_type_list_index<Ty, Ty...>::value]){
                handle.addNormalComponents<Ty>(ID, componentDeserialize<Ty>(component));
                return true;
            }
            return false;
        }());
        my_assert(match, "Invalid Component Named "s + component->name());
    } 
};

template<typename ...Ty>
struct loadComponent<SingletonComponent<Ty...>>{
    void operator()(
        rapidxml::xml_node<char>* root, 
        carphymodel::Components::Modifier& handle,
        const std::vector<string_view>& token_list){
        int tmp[] = {[&](){
            if(auto p = root->first_node(token_list[get_type_list_index<Ty, Ty...>::value].data()); p != 0){
                handle.addSingletonComponents<Ty>(componentDeserialize<Ty>(p));
            }
            return 0;
        }()...};
    }
};

// void loadComponent(size_t ID, rapidxml::xml_node<char>* component, carPhyModel::Components::Modifier& handle){
//     // name must be a string like "Name"sv to enable operator== between string
//     if(component->name() == "Coordinate"sv){
//         handle.addNormalComponents<Coordinate>(ID, componentDeserialize<Coordinate>(component));
//     }else if(component->name() == "ProtectionModel"sv){
//         handle.addNormalComponents<ProtectionModel>(ID, componentDeserialize<ProtectionModel>(component));
//     }else if(component->name() == "DamageModel"sv){
//         handle.addNormalComponents<DamageModel>(ID, componentDeserialize<DamageModel>(component));
//     }else if(component->name() == "FireUnit"sv){
//         handle.addNormalComponents<FireUnit>(ID, componentDeserialize<FireUnit>(component));
//     }else if(component->name() == "SensorData"sv){
//         handle.addNormalComponents<SensorData>(ID, componentDeserialize<SensorData>(component));
//     }else throw BadComponentName("do not have a component named "s + component->name());
// }

}

namespace carphymodel{

void CarBuilder::buildFromSource(const std::string& srcXML, CarModel& model){
    if(model.systems.empty()){
        model.systems.push_back(std::make_unique<PrepareSystem>());

        model.systems.push_back(std::make_unique<SensorSystem>());
        model.systems.push_back(std::make_unique<FireControlSystem>());

        model.systems.push_back(std::make_unique<BallisticSystem>());
        model.systems.push_back(std::make_unique<HitSystem>());

        // model.systems.push_back(std::make_unique<ProtectionSystem>());
        model.systems.push_back(std::make_unique<DamageSystem>());

        model.systems.push_back(std::make_unique<HullSystem>());

        model.systems.push_back(std::make_unique<OutputSystem>());
    }

    //初始化xml解析器
    xml_document<> doc;
    CStyleString s(srcXML);
    doc.parse<parse_default>(s.s);
    auto root=doc.first_node("car");
    
    if(auto handle = model.components.getModifier()){
        handle.addSingletonComponents<
            CommandBuffer,
            EventBuffer,
            DamageModel,
            Coordinate,
            HitEventQueue,
            FireEventQueue,
            ScannedMemory,
            Hull
        >({}, {}, {}, {}, {}, {}, {}, {});

        loadComponent<SingletonComponent<
            WheelMotionParamList
        >>{}(root, handle, {
            "WheelMotionParamList"
        });

        for(auto entity = root->first_node("entity"); entity; entity = entity->next_sibling("entity")){
            auto ID = handle.newEntity();
            for(auto component = entity->first_node(0); component; component = component->next_sibling()){
                loadComponent<NormalComponent<
                    Coordinate,
                    DamageModel,
                    Block,
                    ProtectionModel,
                    FireUnit,
                    SensorData
                >>{}(ID, component, handle, {
                    "Coordinate",
                    "DamageModel",
                    "Block",
                    "ProtectionModel",
                    "FireUnit",
                    "SensorData"
                });
            }
        }
    }
}

}