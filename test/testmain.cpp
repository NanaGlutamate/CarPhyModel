#include "basetest.h"
//#include "test01.hpp"
//#include "test02.hpp"
//#include "test03.hpp"
//#include "test04.hpp"
//#include "test05.hpp"
//#include "test06.hpp"
//#include "test07.hpp"
//#include "test08.hpp"
//#include "test09.hpp"

template<typename T1>
struct A{
    inline static int cnt = 0;
    template<typename T2>
    struct B{
        inline static int cnt = 0;
        B(){if(cnt==0)cnt=++A::cnt;}
    };
    template<typename T3>
    static int count(){
        B<T3> b;
        return b.cnt;
    };
};

struct Arefl{
    constexpr static const char* token_list[] = {"a", "b", "c"};
    int a;
    double b;
    int c;
};

class CStyleString{
public:
    char*s;
    CStyleString(size_t n):s(new char[n]){};
    CStyleString(const CStyleString& s)=delete;
    CStyleString(const std::string& str):s(new char[str.size()+5]){memcpy(s,str.c_str(),str.size()+1);};
    ~CStyleString(){delete[] s;};
};

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
    rapidxml::xml_document<> doc;
    CStyleString s("<A><a>1</a><b>1.0</b></A>");
    doc.parse<rapidxml::parse_default>(s.s);
    auto root=doc.first_node("A");
    Arefl ta = carPhyModel::components::componentDeserialize<Arefl>(root);
    auto tmp = pfr::structure_to_tuple(ta);

    auto ta1 = A<int>::count<int>();
    auto ta2 = A<int>::count<float>();

    auto ta3 = A<float>::count<int>();

    return 0;
};