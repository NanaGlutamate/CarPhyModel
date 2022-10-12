#include "carbuilder.h"

namespace{
using namespace rapidxml;
using namespace std;

class CStyleString{
public:
    char*s;
    CStyleString(size_t n):s(new char[n]){};
    CStyleString(const CStyleString& s)=delete;
    CStyleString(const std::string& str):s(new char[str.size()+5]){memcpy(s,str.c_str(),str.size()+1);};
    ~CStyleString(){delete[] s;};
};

template<typename RETURN_TYPE>
RETURN_TYPE deserialize(xml_node<char>* node);/*{
    using std::string;
    RETURN_TYPE tmp;
    auto args = node->first_node(0);
    while(args){
        tmp.init(string(args->name()), string(args->value()));
        args = args->next_sibling(0);
    }
    return tmp;
};*/

template<> double deserialize<double>(xml_node<char>*node){
    return stof(node->value());
};

template<> int deserialize<int>(xml_node<char>*node){
    return stoi(node->value());
};

template<> std::string deserialize<std::string>(xml_node<char>*node){
    return std::string(node->value());
};

template<> Vector3 deserialize<Vector3>(xml_node<char>*node){
    if(node->first_node("x")==0){
        Quaternion q{deserialize<double>(node->first_node("yaw")),
                     deserialize<double>(node->first_node("pitch")),
                     deserialize<double>(node->first_node("roll"))};
        return Vector3(q.x, q.y, q.z);
    }else{
        return Vector3{deserialize<double>(node->first_node("x")),
                       deserialize<double>(node->first_node("y")),
                       deserialize<double>(node->first_node("z"))};
    }
};

template<> Hull deserialize<Hull>(xml_node<char>*node){
    return Hull{Vector3(0,0,0),
                Vector3(0,0,0),
                deserialize<double>(node->first_node("mass")),
                deserialize<Vector3>(node->first_node("inertia"))};
};

template<> ProtectionModel deserialize<ProtectionModel>(xml_node<char>*node){
    return ProtectionModel{deserialize<double>(node->first_node("armor")),
                           deserialize<int>(node->first_node("activeProtectionAmmo")),
                           deserialize<int>(node->first_node("reactiveArmor")),
                           deserialize<Vector3>(node->first_node("protectZone"))};
};

template<> Coordinate deserialize<Coordinate>(xml_node<char>*node){
    return Coordinate{deserialize<Vector3>(node->first_node("position")),
                      deserialize<Vector3>(node->first_node("altitude"))};
};

template<> PartDamageModel deserialize<PartDamageModel>(xml_node<char>*node){
    return PartDamageModel{DAMAGE_LEVEL::N,
                           deserialize<Vector3>(node->first_node("size"))};
};

template<> Weapon deserialize<Weapon>(xml_node<char>*node){
    return Weapon{deserialize<std::string>(node->first_node("ammoType")),
                  deserialize<int>(node->first_node("ammoRemain")),
                  deserialize<double>(node->first_node("reloadingTime")),
                  0.0f};
};

template<> FireUnit deserialize<FireUnit>(xml_node<char>*node){
    return FireUnit{
        FIRE_UNIT_STATE::FREE,
        {deserialize<double>(node->first_node("fireZone")->first_node("yawLeft")),
        deserialize<double>(node->first_node("fireZone")->first_node("yawRight")),
        deserialize<double>(node->first_node("fireZone")->first_node("pitchUp")),
        deserialize<double>(node->first_node("fireZone")->first_node("pitchDown"))},
        {deserialize<double>(node->first_node("rotateZone")->first_node("yawLeft")),
        deserialize<double>(node->first_node("rotateZone")->first_node("yawRight")),
        deserialize<double>(node->first_node("rotateZone")->first_node("pitchUp")),
        deserialize<double>(node->first_node("rotateZone")->first_node("pitchDown"))},
        {0.0f,0.0f},
        {deserialize<double>(node->first_node("rotateSpeed")->first_node("yaw")),
        deserialize<double>(node->first_node("rotateSpeed")->first_node("pitch"))},
        deserialize<Weapon>(node->first_node("weapon")),
    };
};

template<> SensorData deserialize<SensorData>(xml_node<char>*node){
    return SensorData{deserialize<std::string>(node->first_node("type"))};
};

template<> WheelMotionParamList deserialize<WheelMotionParamList>(xml_node<char>*node){
    return WheelMotionParamList{
        0.,
        deserialize<double>(node->first_node("length")),
        deserialize<double>(node->first_node("maxAngle")),
        deserialize<double>(node->first_node("rotateSpeed")),
        deserialize<double>(node->first_node("maxLinearSpeed")),
        deserialize<double>(node->first_node("maxFrontAcceleration")),
        deserialize<double>(node->first_node("maxBrakeAcceleration")),
        deserialize<double>(node->first_node("maxLateralAcceleration")),
    };
};

};

void CarBuilder::build(const std::string& srcXML, CarModel& model, bool need_erase){
    //初始化xml解析器
    xml_document<> doc;
    CStyleString s(srcXML);
    doc.parse<parse_default>(s.s);
    auto root=doc.first_node("car");

    //清空残留值
    if(need_erase){
        model.coordinate.erase(model.coordinate.begin(), model.coordinate.end());
        model.protection.erase(model.protection.begin(), model.protection.end());
        model.damage.erase(model.damage.begin(), model.damage.end());
        model.fireUnit.erase(model.fireUnit.begin(), model.fireUnit.end());
        model.sensor.erase(model.sensor.begin(), model.sensor.end());
    }

    map<string, CID> registration; //每个部件名称对应的ID
    CID i=0; //当前ID

    //解析运动模型
    model.hull=deserialize<Hull>(root->first_node("hull"));
    if(deserialize<string>(root->first_node("hull")->first_node("type"))==string("wheeled")){
        model.params=deserialize<WheelMotionParamList>(root->first_node("hull"));
    }

    //解析毁伤模型
    auto node=root->first_node("damagePartList")->first_node("damagePart");
    while(node!=0){
        model.damage.push_back({i, deserialize<PartDamageModel>(node)});
        model.coordinate.push_back({i, deserialize<Coordinate>(node)});
        //XML中同一个部分的毁伤模型和火力/传感器模型通过name联系，而模型中通过ID联系，因此需要保存name对应ID便于查找
        registration[string(node->first_node("name")->value())]=i;
        ++i;
        node=node->next_sibling("damagePart");
    }

    //解析防护模型
    node=root->first_node("protectionList")->first_node("protection");
    while(node!=0){
        model.protection.push_back({i, deserialize<ProtectionModel>(node)});
        model.coordinate.push_back({i, deserialize<Coordinate>(node)});
        ++i;
        node=node->next_sibling("protection");
    }

    //解析火力单元
    node=root->first_node("fireUnitList")->first_node("fireUnit");
    while(node!=0){
        string name{node->first_node("damageModelName")->value()};
        model.fireUnit.push_back({registration[name], deserialize<FireUnit>(node)});
        node=node->next_sibling("fireUnit");
    }

    //解析传感器单元
    node=root->first_node("sensorList")->first_node("sensor");
    while(node!=0){
        string name{node->first_node("damageModelName")->value()};
        model.sensor.push_back({registration[name], deserialize<SensorData>(node)});
        node=node->next_sibling("sensor");
    }
};