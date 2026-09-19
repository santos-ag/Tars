#pragma once
#include "Types.hpp"
#include <cmath>
#include <vector>
struct Layer {
    int in;
    int out;

    m<float> weights;
    v<float> activations;
    v<float> bias;

    Layer(int in, int out)
        : in(in), out(out), weights(out, std::vector<float>(in)), activations(in, 0), bias(out, 0) {
        // Preenche cada peso com um valor aleatório entre [-1,1]
        for (auto &w : weights) {
            for (auto &y : w) {
                y = ((float)rand() / (float)RAND_MAX) * 2 - 1;
            }
        }
    }
};
