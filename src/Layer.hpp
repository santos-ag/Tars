#pragma once
#include "Types.hpp"
#include <cmath>
#include <vector>
struct Layer {
    i32 in;
    i32 out;

    m<f32> weights;
    v<f32> activations;
    v<f32> bias;

    Layer(i32 in, int out)
        : in(in), out(out), weights(out, v<f32>(in)), activations(in, 0), bias(out, 0) {
        // Preenche cada peso com um valor aleatório entre [-1,1]
        for (auto &w : weights) {
            for (auto &y : w) {
                y = ((f32)rand() / (float)RAND_MAX) * 2 - 1;
            }
        }
    }
};
