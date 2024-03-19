#pragma once

#include <random>

namespace carphymodel {

inline bool testRandom(double prob) {
    static auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine e(static_cast<uint32_t>(seed));
    static std::uniform_real_distribution<double> u(0., 1.);
    return u(e) < prob;
}

} // namespace carphymodel