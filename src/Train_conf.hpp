#pragma once
#include "NeuralNetwork.hpp"
#include <array>
#include <cmath>
#include <time.h>
#include <vector>

namespace conf {
inline int epochs = 10;
inline float lr = 1e-3;
inline float h = 1e-4;

struct data {
    std::vector<float> x;
    float y;
};

inline std::vector<int> topology = {2, 1};

inline std::vector<data> data_tr = {{{0, 0}, 0}, {{1, 0}, 1}, {{0, 1}, 1}, {{1, 1}, 1}};
} // namespace conf
