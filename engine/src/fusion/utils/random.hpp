#pragma once

#include <random>

namespace fe {
    class Random {
    public:
        static void Init() {
            RandomEngine.seed(std::random_device()());
        }

        static float Float() {
            return Distribution(RandomEngine);
        }

    private:
        static std::mt19937 RandomEngine;
        static std::normal_distribution<float> Distribution;
    };
}
