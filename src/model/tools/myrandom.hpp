#pragma once

#include <chrono>
#include <random>

namespace carphymodel {

/**
 * @brief return random number in [0.0, 1.0)
 * 
 * @return double random number
 */
inline double rand() {
    static auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine e(seed);
    static std::uniform_real_distribution<double> u(0, 1.);
    return u(e);
}

} // namespace carphymodel