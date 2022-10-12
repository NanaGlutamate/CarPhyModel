#pragma once

#include <map>
#include <memory>
#include <string>

// template <typename T>
// class Singleton : private T{
// private:
//     Singleton();
// public:
//     static T* getInstance(){
//         static Singleton t;
//         return (T*)&t;
//     };
// };

template <typename T>
class Factory{
public:
    using LUT = std::map<std::string, std::shared_ptr<T>>;
private:
    Factory(){};
    static LUT look_up_table;
public:
    static void registerProduct(std::string name, const T& value){
        look_up_table.emplace(name, std::make_shared<T>(value));
    };
    static std::weak_ptr<T> getProduct(std::string name){
        auto it = look_up_table.find(name);
        assert(it != look_up_table.end());
        return std::weak_ptr<T>(it->second);
    };
};