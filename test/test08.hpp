#include "basetest.h"
#include "../src/model/framework/componentmanager.hpp"
#include <tuple>

namespace test08{

    int test_main() {

        using namespace std;

        ComponentManager<SingletonComponent<int, double>, NormalComponent<float, short, char, int>> manager;

        auto tmp1 = manager.hasSingletonComponent<int>();
        manager.addSingletonComponent<int>(0);
        auto tmp2 = manager.hasSingletonComponent<int>();
        auto tmp3 = manager.hasSingletonComponent<int, double>();

        manager.addNormalComponent<float>(0, 0.);
        manager.addNormalComponent<float>(1, 0.);
        manager.addNormalComponent<float>(2, 0.);
        manager.addNormalComponent<float>(3, 0.);
        manager.addNormalComponent<float>(4, 0.);
        
        manager.addNormalComponent<short>(2, 0);
        manager.addNormalComponent<short>(3, 0);
        manager.addNormalComponent<short>(4, 0);
        
        manager.addNormalComponent<char>(1, 0);
        manager.addNormalComponent<char>(2, 0);

        manager.addNormalComponent<float, char>(5, 1., 1);

        for(auto& [i, f, s] : manager.getNormalComponent<float, short>()){
            f+=1.;
            s+=1;
        }

        for(auto& [i, f, c, s] : manager.getNormalComponent<float, char, short>()){
            c = char(f+s);
        }

        for(auto& s : manager.getNormalComponent<int, float, short, char>()){
            // do nothing
            assert(false);
        }

        manager.ereaseNormalComponent<float, char>(1);
        manager.ereaseNormalComponent<float, char>(1);
        manager.ereaseNormalComponent<float, char>(0);

        manager.ereaseSingletonComponent<int>();

        std::get<0>(manager.getSingletonComponent<int>()) = 1;

        std::get<0>(manager.getSingletonComponent<double>()) = 1.;

        return 0;
    }

}