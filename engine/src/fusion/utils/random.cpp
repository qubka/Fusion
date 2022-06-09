#include "random.hpp"

using namespace fe;

std::mt19937 Random::RandomEngine;
std::normal_distribution<float> Random::Distribution;