#pragma once
#include <cstdlib>

#include <vector>

struct Layer {
    int in;
    int out;

    std::vector<float> weights;
    std::vector<float> bias;

    Layer(int in_dim, int out_dim)
        : in(in_dim), out(out_dim), weights(in_dim * out_dim), bias(out_dim, 0) {

        // Preenche cada peso com um valer entre [-1,1] inclusivo;
        for (auto w : weights) {
            w = ((float)rand() / RAND_MAX) * 2 - 1;
        }
    }
};
