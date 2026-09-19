#pragma once
#include "NeuralNetwork.hpp"
#include <array>
#include <cmath>
#include <time.h>
#include <vector>

namespace conf {
inline int epochs = 10000;
inline float lr = 1e-1;
inline float h = 1e-4;

struct data {
    v<float> x;
    float y;
};

inline v<int> topology = {2, 1};

inline v<data> data_tr = {{{0, 0}, 0}, {{1, 0}, 1}, {{0, 1}, 1}, {{1, 1}, 1}};
} // namespace conf
