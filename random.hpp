#pragma once

#include <random>

template <class DIST = std::uniform_int_distribution<int>>
class RandomGenerator {
public:
    RandomGenerator(std::random_device& rd, typename DIST::result_type min, typename DIST::result_type max) : rd(rd), dist(min, max) {}

    typename DIST::result_type operator()() {
        return dist(rd);
    }

private:
    DIST dist;
    std::random_device& rd;
};
