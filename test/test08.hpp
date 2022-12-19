#include "basetest.h"
#include "../src/model/framework/componentmanager.hpp"
#include <tuple>

namespace test08{

    int test_main() {

        using namespace std;

        ComponentManager<SingletonComponent<int, double>, NormalComponent<float, short, char, int>> manager;

        if (auto handle = manager.getModifier()) {
            handle.addSingletonComponents<int>(0);
            handle.addNormalComponents<float>(0, 0.);
            handle.addNormalComponents<float>(1, 0.);
            handle.addNormalComponents<float>(2, 0.);
            handle.addNormalComponents<float>(3, 0.);
            handle.addNormalComponents<float>(4, 0.);
            handle.addNormalComponents<short>(2, 0);
            handle.addNormalComponents<short>(3, 0);
            handle.addNormalComponents<short>(4, 0);
            handle.addNormalComponents<char>(1, 0);
            handle.addNormalComponents<char>(2, 0);
            handle.addNormalComponents<float, char>(5, 1., 1);
        }


        for(auto&& [i, f, s] : manager.getNormalComponents<float, short>()){
            f+=1.;
            s+=1;
        }

        for(auto&& [i, f, c, s] : manager.getNormalComponents<float, char, short>()){
            c = char(f+s);
        }

        for(auto&& s : manager.getNormalComponents<int, float, short, char>()){
            // do nothing
            assert(false);
        }

        return 0;
    }

}