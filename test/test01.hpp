#include "basetest.h"

namespace test01{
//XML parser test

class CStyleString{
public:
    char*s;
    CStyleString(size_t n):s(new char[n]){};
    CStyleString(const std::string& str):s(new char[str.size()+5]){memcpy(s,str.c_str(),str.size()+5);};
    ~CStyleString(){delete[] s;};
};

int test_main(){
    using namespace std;
    using namespace rapidxml;
    xml_document<> doc;
    string s="<car><vehicle><wheel/><wheel>f</wheel><wheel>b</wheel></vehicle></car>    ";
    CStyleString str(s);
    doc.parse<parse_default>(str.s);
    auto node=doc.first_node("car");
    auto t1 = node->value();
    auto t2 = node->first_node()->value();
    auto t3 = node->first_node()->first_node()->value();
    cout<<str.s;
    return 0;
};
}