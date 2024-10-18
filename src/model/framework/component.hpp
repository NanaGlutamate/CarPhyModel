#pragma once

#include "../tools/datastructure.hpp"
#include "src/extern/pfr.hpp"
#include "src/extern/rapidxml-1.13/rapidxml.hpp"
#include <string>
#include <type_traits>

namespace carphymodel::component {

#if __cplusplus < 202002L

template <typename Ty>
using remove_cvref = typename std::remove_cv<typename std::remove_reference<Ty>::type>::type;

#else

using std::remove_cvref;

#endif

// #if __cplusplus >= 202002L

// template<typename T>
// concept Component =
//     std::is_same_v<T, int> ||
//     std::is_same_v<T, float> ||
//     std::is_same_v<T, double> ||
//     std::is_enum_v<T> ||
//     requires{
//         std::string(T::token_list[0]);
//         pfr::get<0>(T{});
//     };

// #define Restrict Component

// #else

// #define Restrict typename

// #endif

template <typename RETURN_TYPE>
inline RETURN_TYPE componentDeserialize(rapidxml::xml_node<char>* node);

template <typename Tar>
void emplaceIfNotNull(Tar& tar, rapidxml::xml_node<char>* node) {
    if(node){
        tar = componentDeserialize<Tar>(node);
    }
}

//template <typename RETURN_TYPE>
//inline RETURN_TYPE componentDeserialize(rapidxml::xml_node<char>* node) {
//    static_assert(false, "not supported type");
//}

// token starts with '!' : no deserialize
// token starts with '-' : default deserialize
// template <typename T, size_t Index>
// inline void emplace(T& tar, rapidxml::xml_node<char>* node) {
//     if constexpr (Index < pfr::tuple_size_v<T>) {
//         if constexpr (T::token_list[Index][0] != '!') {
//             if constexpr (T::token_list[Index][0] == '-') {
//                 // if constexpr (std::is_enum_v<decltype(pfr::get<Index>(tar))>) {
//                 //     pfr::get<Index>(tar) = static_cast<decltype(pfr::get<Index>(tar))>(0);
//                 // } else {
//                 pfr::get<Index>(tar) = {};
//                 //}
//             } else {
//                 auto tmp = node->first_node(T::token_list[Index]);
//                 if (tmp != 0) {
//                     pfr::get<Index>(tar) =
//                         componentDeserialize<typename remove_cvref<decltype(pfr::get<Index>(tar))>::type>(tmp);
//                 } else {
//                     pfr::get<Index>(tar) = {};
//                 }
//                 emplace<T, Index + 1>(tar, node);
//             }
//         }
//     }
// }

// template <typename Ty>
// inline constexpr bool is_vector_v = false;
// template <typename T, typename A>
// inline constexpr bool is_vector_v<std::vector<T, A>> = true;

// template <typename RETURN_TYPE>
// inline RETURN_TYPE componentDeserialize(rapidxml::xml_node<char>* node) {
//     if constexpr (std::is_enum_v<RETURN_TYPE>) {
//         return (RETURN_TYPE)componentDeserialize<int>(node);
//     } else if constexpr (is_vector_v<RETURN_TYPE>) {
//         RETURN_TYPE l;
//         decltype(RETURN_TYPE{}[0]) tmp;
//         auto name = node->name();
//         for (; node; node = node->next_sibling(name)) {
//             emplace<typename remove_cvref<decltype(RETURN_TYPE{}[0])>::type, 0>(tmp, node);
//             l.push_back(tmp);
//         }
//         return l;
//     } else {
//         RETURN_TYPE tmp = RETURN_TYPE::make();
//         emplace<RETURN_TYPE, 0>(tmp, node);
//         return tmp;
//     }
// }

template <>
inline double componentDeserialize<double>(rapidxml::xml_node<char>* node) {
    if (!node) {
        return 0.;
    }
    return std::stof(node->value());
}

template <>
inline float componentDeserialize<float>(rapidxml::xml_node<char>* node) {
    return std::stof(node->value());
}

template <>
inline int componentDeserialize<int>(rapidxml::xml_node<char>* node) {
    if (!node) {
        return 0;
    }
    return std::stoi(node->value());
}

template <>
inline std::string componentDeserialize<std::string>(rapidxml::xml_node<char>* node) {
    return std::string(node->value());
}

template <>
inline Vector3 componentDeserialize<Vector3>(rapidxml::xml_node<char>* node) {
    if (node->first_node("x") != 0) {
        return Vector3{std::stof(node->first_node("x")->value()), std::stof(node->first_node("y")->value()),
                       std::stof(node->first_node("z")->value())};
    } else {
        Quaternion q(std::stof(node->first_node("roll")->value()), std::stof(node->first_node("pitch")->value()),
                     std::stof(node->first_node("yaw")->value()));
        return q.toCompressedQuaternion();
    }
}

template <>
inline Coordinate componentDeserialize<Coordinate>(rapidxml::xml_node<char>* node) {
    return Coordinate{componentDeserialize<Vector3>(node->first_node("position")),
                      componentDeserialize<Vector3>(node->first_node("attitude"))};
}

template <>
inline Block componentDeserialize<Block>(rapidxml::xml_node<char>* node) {
    Block tmp = Block::make();
    tmp.length = componentDeserialize<double>(node->first_node("length"));
    tmp.width = componentDeserialize<double>(node->first_node("width"));
    tmp.height = componentDeserialize<double>(node->first_node("height"));
    return tmp;
}

template <>
inline ProtectionModel componentDeserialize<ProtectionModel>(rapidxml::xml_node<char>* node) {
    ProtectionModel tmp = ProtectionModel::make();
    tmp.armor_front = componentDeserialize<double>(node->first_node("armor_front"));
    tmp.armor_back = componentDeserialize<double>(node->first_node("armor_back"));
    tmp.armor_side = componentDeserialize<double>(node->first_node("armor_side"));
    tmp.armor_bottom = componentDeserialize<double>(node->first_node("armor_bottom"));
    tmp.armor_top = componentDeserialize<double>(node->first_node("armor_top"));
    tmp.activeProtectionAmmo = componentDeserialize<int>(node->first_node("activeProtectionAmmo"));
    tmp.reactiveArmor = componentDeserialize<int>(node->first_node("reactiveArmor"));
    tmp.coverageRate = componentDeserialize<double>(node->first_node("coverageRate"));
    tmp.jammer = componentDeserialize<double>(node->first_node("jammer"));
    tmp.hidden = componentDeserialize<double>(node->first_node("hidden"));
    tmp.Interception_probability1 = componentDeserialize<double>(node->first_node("Interception_probability1"));
    tmp.Interception_probability2 = componentDeserialize<double>(node->first_node("Interception_probability2"));
    tmp.active_interference_rate = componentDeserialize<double>(node->first_node("active_interference_rate"));
    tmp.active_interference_distance = componentDeserialize<double>(node->first_node("active_interference_distance"));
    return tmp;
}

template <>
inline DamageModel componentDeserialize<DamageModel>(rapidxml::xml_node<char>* node) {
    DamageModel tmp = DamageModel::make();
    tmp.repairTime_K = componentDeserialize<double>(node->first_node("repairTime_K"));
    tmp.repairTime_KK = componentDeserialize<double>(node->first_node("repairTime_KK"));
    emplaceIfNotNull(tmp.maxInfluence, node->first_node("maxInfluence"));
    return tmp;
}

template <>
inline DAMAGE_LEVEL componentDeserialize<DAMAGE_LEVEL>(rapidxml::xml_node<char>* node) {
    DAMAGE_LEVEL tmp = DAMAGE_LEVEL::KK;
    auto content = node->value();
    while (isspace(*content)) {
        content++;
    }
    if (isdigit(*content)) {
        tmp = static_cast<DAMAGE_LEVEL>(componentDeserialize<int>(node));
    } else if (content[0] == 'N'){
        tmp = DAMAGE_LEVEL::N;
    } else if (content[0] == 'M') {
        tmp = DAMAGE_LEVEL::M;
    } else if (content[0] == 'K' && content[1] != 'K') {
        tmp = DAMAGE_LEVEL::K;
    }
    return tmp;
}

template <>
inline Direction componentDeserialize<Direction>(rapidxml::xml_node<char>* node) {
    Direction tmp = Direction::make();
    tmp.yaw = componentDeserialize<double>(node->first_node("yaw"));
    tmp.pitch = componentDeserialize<double>(node->first_node("pitch"));
    return tmp;
}

template <>
inline AngleZone componentDeserialize<AngleZone>(rapidxml::xml_node<char>* node) {
    AngleZone tmp = AngleZone::make();
    tmp.yawLeft = componentDeserialize<double>(node->first_node("yawLeft"));
    tmp.yawRight = componentDeserialize<double>(node->first_node("yawRight"));
    tmp.pitchUp = componentDeserialize<double>(node->first_node("pitchUp"));
    tmp.pitchDown = componentDeserialize<double>(node->first_node("pitchDown"));
    return tmp;
}

template <>
inline Weapon componentDeserialize<Weapon>(rapidxml::xml_node<char>* node) {
    Weapon tmp = Weapon::make();
    tmp.ammoType = componentDeserialize<std::string>(node->first_node("ammoType"));
    tmp.ammoRemain = componentDeserialize<int>(node->first_node("ammoRemain"));
    tmp.reloadingTime = componentDeserialize<double>(node->first_node("reloadingTime"));
    tmp.range = componentDeserialize<double>(node->first_node("range"));
    tmp.speed = componentDeserialize<double>(node->first_node("speed"));
    tmp.param1 = componentDeserialize<double>(node->first_node("param1"));
    tmp.param2 = componentDeserialize<double>(node->first_node("param2"));
    tmp.ammototal = tmp.ammoRemain;
    return tmp;
}

template <>
inline FireUnit componentDeserialize<FireUnit>(rapidxml::xml_node<char>* node) {
    FireUnit tmp = FireUnit::make();
    tmp.fireZone = componentDeserialize<AngleZone>(node->first_node("fireZone"));
    tmp.rotateZone = componentDeserialize<AngleZone>(node->first_node("rotateZone"));
    tmp.rotateSpeed = componentDeserialize<Direction>(node->first_node("rotateSpeed"));
    tmp.weapon = componentDeserialize<Weapon>(node->first_node("weapon"));
    return tmp;
}

template <>
inline SensorData componentDeserialize<SensorData>(rapidxml::xml_node<char>* node) {
    SensorData tmp = SensorData::make();
    tmp.type = componentDeserialize<std::string>(node->first_node("type"));
    tmp.detectrange = componentDeserialize<double>(node->first_node("detectrange"));
    tmp.detectprobability = componentDeserialize<double>(node->first_node("detectprobability"));
    tmp.target_positioning_accuracy = componentDeserialize<double>(node->first_node("target_positioning_accuracy"));
    return tmp;
}

template <>
inline CommunicationData componentDeserialize<CommunicationData>(rapidxml::xml_node<char>* node) {
    CommunicationData tmp = CommunicationData::make();
    tmp.type = componentDeserialize<std::string>(node->first_node("type"));
    return tmp;
}

template <>
inline WheelMotionParamList componentDeserialize<WheelMotionParamList>(rapidxml::xml_node<char>* node) {
    WheelMotionParamList tmp = WheelMotionParamList::make();
    tmp.LENGTH = componentDeserialize<double>(node->first_node("LENGTH"));
    tmp.MAX_ANGLE = componentDeserialize<double>(node->first_node("MAX_ANGLE"));
    tmp.ROTATE_SPEED = componentDeserialize<double>(node->first_node("ROTATE_SPEED"));
    tmp.MAX_LINEAR_SPEED = componentDeserialize<double>(node->first_node("MAX_LINEAR_SPEED"));
    tmp.MAX_FRONT_ACCELERATION = componentDeserialize<double>(node->first_node("MAX_FRONT_ACCELERATION"));
    tmp.MAX_BRAKE_ACCELERATION = componentDeserialize<double>(node->first_node("MAX_BRAKE_ACCELERATION"));
    tmp.MAX_LATERAL_ACCELERATION = componentDeserialize<double>(node->first_node("MAX_LATERAL_ACCELERATION"));
    tmp.OIL_REMAIN = componentDeserialize<double>(node->first_node("OIL_REMAIN"));
    tmp.OIL_CONSUMPTION = componentDeserialize<double>(node->first_node("OIL_CONSUMPTION"));
    tmp.MAX_CLIMBING_ACCELERATION = componentDeserialize<double>(node->first_node("MAX_CLIMBING_ACCELERATION"));
    return tmp;
}

// template<typename T>
// inline std::vector<T> componentDeserialize<std::vector<T>>(rapidxml::xml_node<char>* node){
//     std::vector<T> tmp;
//     auto child = node->first_node(0);
//     while(child){
//         tmp.push_back(componentDeserialize<T>(child));
//         child = child->next_sibling(0);
//     }
//     return tmp;
// }

} // namespace carphymodel::component