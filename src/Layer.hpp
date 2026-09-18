#pragma once
#include <cstdlib>

#include <vector>

struct Layer {
    int in;
    int out;

    std::vector<std::vector<float>> weights;
    std::vector<float> activations;
    std::vector<float> bias;

    Layer(int in, int out)
        : in(in), out(out), weights(in, std::vector<float>(out)), activations(in, 0), bias(out, 0) {
        // Preenche cada peso com um valor aleatório entre [-1,1]
        for (auto &w : weights) {
            for (auto &y : w) {
                y = ((float)rand() / RAND_MAX) * 2 - 1;
            }
        }
    }
};
