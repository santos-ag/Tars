#pragma once
#include <array>
#include <vector>

namespace conf {

const int epochs = 5;

const float h = 1e-4f;

std::vector<std::array<float, 2>> data_tr = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};

const float lr = 1e1;

float oracle(std::array<float, 2> &activations);
} // namespace conf
