#include "basetest.h"
#include <cmath>

namespace test04{
//entity synthesizer test

struct A{
    int a;
    int b;
};

struct B{
    int c;
};

std::ostream& operator<<(std::ostream& os, const A& a){
    return os << "(" << a.a << "," << a.b << ")";
}

std::ostream& operator<<(std::ostream& os, const B& b){
    return os << "(" << b.c << ")";
}

int test_main(){
    using namespace std;

    vector<tuple<size_t, A>> va{{0,{1,2}},{1,{3,4}},{2,{5,6}}};
    vector<tuple<size_t, B>> vb{{0,{1}},{1,{2}},{2,{3}}};
    vector<tuple<size_t, B>> vc{{1,{0}},{0,{1}},{2,{2}}};
    IDMatcher<size_t, A, B, B> matcher(va, vb, vc);
    auto t=matcher.find(1);
    
    EntitySynthesizer<size_t, A, B, B> es(va, vb, vc);

    for(auto& [i, a, b, c] : es){
        cout<<a<<" "<<b<<" "<<c<<endl;
        a.a = b.c + c.c;
    }

    //调试断点

    cout<<endl;

    for(auto& [i, a, b, c] : make_entity(va, vb, vc)){
        cout<<a<<" "<<b<<" "<<c<<endl;
        a.a = b.c - c.c;
    }
    
    //调试断点

    return 0;
};
};