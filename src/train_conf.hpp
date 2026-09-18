#pragma once
#include <array>
#include <cmath>
#include <time.h>
#include <vector>
namespace conf {

const int epochs = 5;

const float h = 1e-4f;
struct data {
    std::array<float, 2> x;
    float y;
};
std::vector<data> data_tr = {{{0, 0}, 0}, {{1, 0}, 1}, {{0, 1}, 1}, {{1, 1}, 1}};

const float lr = 1e1;
// srand(time(nullptr));
std::vector<float> params = {1, 1, 1};
} // namespace conf
