#pragma once

#include <map>
#include <memory>
#include <string>

namespace carphymodel{

template <typename T>
class Singleton{
public:
    static T& getInstance(){
        static T t;
        return t;
    };
};

template <typename Fac, typename T>
class Factory{
public:
    using LUT = std::map<std::string_view, std::shared_ptr<T>>;
    static void emplaceProduct(std::string_view name, std::shared_ptr<T> value){
        Fac::look_up_table.emplace(name, value);
    };
    static T* getProduct(std::string_view name){
        auto it = Fac::look_up_table.find(name);
        return it->second.get();
    };
private:
    Factory() = delete;
};

};