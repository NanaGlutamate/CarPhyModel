// #include <iostream>
#include "basetest.h"
//#include "test01.hpp"
#include "test02.hpp"
//#include "test03.hpp"
//#include "test04.hpp"
//#include "test05.hpp"
//#include "test06.hpp"
//#include "test07.hpp"
#include "test08.hpp"
//#include "test09.hpp"
#include "test11.hpp"

int main(){
    //test01::test_main();
    //test02::test_main();
    //test03::test_main();
    //test04::test_main();
    //test05::test_main();
    //test06::test_main();
    //test07::test_main();
    //test08::test_main();
    //test09::test_main();
    //test11::test_main();
    std::vector<double> tmp{1, 2, 3, 4, 5};
    std::vector<double> tmp1;
    // std::copy_if(tmp.begin(), tmp.end(), tmp1.begin(), [](double x){return true;});
    std::erase_if(tmp, [](double x){
        return x>=2.5 && x<=3.5;
    });

    // struct{int a; float b;} a;
    // auto [i, j] = a;

    return 0;
};