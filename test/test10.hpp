#pragma once

namespace test10{

template<typename T1>
struct A{
    inline static int cnta = 0;
    template<typename T2>
    struct B{
        inline static int cntb = 0;
        B(){if(cntb==0)cntb=++A<T1>::cnta;}
    };
    template<typename T3>
    void count(){
        using namespace std;
        B<T3> b;
        cout << "A: " << cnta << ", B: " << b.cntb << endl;
    };
};

int test_main(){
    A<int>{}.count<int>();
    A<int>{}.count<float>();
    A<float>{}.count<float>();
}

}