#include "carbuilder.h"
#include <algorithm>
#include "framework/component.hpp"
#include "src/extern/rapidxml-1.13/rapidxml.hpp"
#include "protection/carprotection.h"
#include "damage/cardamage.h"

namespace{

using namespace rapidxml;
using namespace std;
using namespace carPhyModel;
using namespace carPhyModel::component;

class BadComponentName : public runtime_error {
public:
    BadComponentName(const string& what): runtime_error(what){};
};

class CStyleString{
public:
    char*s;
    CStyleString(size_t n):s(new char[n]){};
    CStyleString(const CStyleString& s)=delete;
    CStyleString(const std::string& str):s(new char[str.size()+5]){memcpy(s,str.c_str(),str.size()+1);};
    ~CStyleString(){delete[] s;};
};

void loadComponent(size_t ID, rapidxml::xml_node<char>* component, carPhyModel::Components::Modifier& handle){
    // name must be a string like "Name"sv to enable operator== between string
    if(component->name() == "Coordinate"sv){
        handle.addNormalComponents<Coordinate>(ID, componentDeserialize<Coordinate>(component));
    }else if(component->name() == "ProtectionModel"sv){
        handle.addNormalComponents<ProtectionModel>(ID, componentDeserialize<ProtectionModel>(component));
    }else if(component->name() == "PartDamageModel"sv){
        handle.addNormalComponents<PartDamageModel>(ID, componentDeserialize<PartDamageModel>(component));
    }else if(component->name() == "FireUnit"sv){
        handle.addNormalComponents<FireUnit>(ID, componentDeserialize<FireUnit>(component));
    }else if(component->name() == "SensorData"sv){
        handle.addNormalComponents<SensorData>(ID, componentDeserialize<SensorData>(component));
    }else throw BadComponentName("do not have a component named "s + component->name());
}

}

namespace carPhyModel{

void CarBuilder::buildFromSource(const std::string& srcXML, CarModel& model){
    //初始化xml解析器
    xml_document<> doc;
    CStyleString s(srcXML);
    doc.parse<parse_default>(s.s);
    auto root=doc.first_node("car");

    if(CarModel::systems.empty()){
        CarModel::systems = {
            make_shared<ProtectionSystem>(),
            make_shared<DamageSystem>(),
        };
    }
    if(auto handle = model.components.getModifier()){
        handle.addSingletonComponents<
            OutputBuffer,
            Coordinate,
            FireEventQueue,
            ScannedMemory,
            Hull
        >({}, {}, {}, {}, {});
        // TODO: wheel motion

        for(auto entity = root->first_node("entity"); entity; entity = entity->next_sibling("entity")){
            auto ID = handle.newEntity();
            for(auto component = entity->first_node(0); component; component = component->next_sibling()){
                loadComponent(ID, component, handle);
            }
        }
    }
};

}