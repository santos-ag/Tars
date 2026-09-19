#pragma once
#include "NeuralNetwork.hpp"
#include <array>
#include <cmath>
#include <time.h>
#include <vector>

namespace conf {
inline int epochs = 1000;
inline float lr = 1e-3;
inline float h = 1e-4;

struct data {
    std::vector<float> x;
    float y;
};

inline std::vector<int> topology = {1, 1};

inline std::vector<data> data_tr = {{{0}, 7},  {{1}, 9},  {{2}, 11}, {{3}, 13},
                                    {{4}, 15}, {{5}, 17}, {{6}, 19}, {{7}, 21},
                                    {{8}, 23}, {{9}, 25}, {{10}, 27}};
} // namespace conf
