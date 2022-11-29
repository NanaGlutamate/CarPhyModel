#include "test/basetest.h"
#include "../src/model/framework/pattern.hpp"

class A{
public:
    virtual void t(){std::cout<<"A"<<std::endl;};
};

class B : public A{
public:
    virtual void t(){std::cout<<"B"<<std::endl;};
};

using ABFactory = Factory<A>;

inline ABFactory::LUT ABFactory::look_up_table{
    {"A", std::shared_ptr<A>(new A())},
    {"B", std::shared_ptr<A>((A*)new B())}
};

namespace test09{

    int test_main() {

        using namespace std;

        ABFactory::getProduct("A").lock()->t();

        ABFactory::getProduct("B").lock()->t();

        return 0;
    }

}