#pragma once
#include "NeuralNetwork.hpp"
#include "Types.hpp"
#include <array>
#include <cmath>
#include <time.h>
#include <vector>

namespace conf {

inline i32 epochs = 1000000;
inline f32 lr = 1e1;
inline f32 h = 1e-4;

struct data {
    v<f32> x;
    f32 y;
};

inline v<i32> topology = {2, 2, 1};

inline v<data> data_tr = {{{0, 0}, 0}, {{1, 0}, 1}, {{0, 1}, 1}, {{1, 1}, 0}};
} // namespace conf
