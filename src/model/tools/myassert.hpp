#pragma once

#if __cplusplus >= 202002L

#include <string>
#include <source_location>
#include <iostream>
#include <format>

void my_assert(bool check,
    const std::string_view message = "",
    const std::source_location location = std::source_location::current())
{
    using namespace std;
    if(!check){
        cout << format("error: {}\nin file {}, line {}", message, location.file_name(), location.line()) << endl;
        abort();//throw std::runtime_error{""};
    }
}

#else

#define my_assert(check, message) assert(check)

#endif