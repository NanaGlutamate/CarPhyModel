#pragma once

#include <map>
#include <memory>
#include <string>

// template <typename T>
// class Singleton{
// protected:
//     Singleton();
// public:
//     static T* getInstance(){
//         static Singleton t;
//         return (T*)&t;
//     };
// };

namespace carPhyModel{

template <typename T>
class Factory{
public:
    using LUT = std::map<std::string, std::shared_ptr<T>>;
private:
    Factory(){};
    static LUT look_up_table;
public:
    static void emplaceProduct(std::string name, std::shared_ptr<T> value){
        look_up_table.emplace(name, value);
    };
    static std::weak_ptr<T> getProduct(std::string name){
        auto it = look_up_table.find(name);
        return it->second;
    };
};

};