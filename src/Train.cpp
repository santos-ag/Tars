#include "Train.hpp"
#include "Layer.hpp"
#include "NeuralNetwork.hpp"
#include "Train_conf.hpp"
#include <iostream>
#include <vector>

float Train::sigmoidf(float x) {
    return 1.0f / (1.0f + exp(-x));
}

float Train::ReLU(float x) {
    return (x > 0.0) ? x : 0.0;
}

float Train::forward(std::array<float, 2> &activations, Layer &l) {
    float z = params[params.size() - 1];
    for (int i = 0; i < params.size() - 1; ++i) {
        z += params[i] * activations[i];
    }
    return sigmoidf(z);
}

float Train::cost(Layer &l) {
    float result = 0;
    for (int i = 0; i < conf::data_tr.size(); ++i) {
        float y = conf::data_tr[i].y;
        float p = forward(conf::data_tr[i].x, l);
        float d = y - p;
        result += d * d;
    }
    return result / conf::data_tr.size();
}

std::vector<std::vector<float>> Train::bgd(Layer &l) {
    auto params = l.weights;
    std::vector<std::vector<float>> derivates(params.size(), std::vector<float>(params[0].size()));

    for (int i = 0; i < derivates.size(); i++) {
        for (int j = 0; j < derivates[0].size(); j++) {
            float temp = params[i][j];
            params[i][j] = temp + conf::h;
            float costp = cost(l);
            params[i][j] = temp - conf::h;
            float costm = cost(l);
            derivates[i][j] = (costp - costm) / (2 * conf::h);
            params[i][j] = temp;
        }
    }
    return derivates;
}

void Train::optimizer(NeuralNetwork model, float lr) {
    auto grad = bgd(model);
    for (auto &l : model.get_layers()) {
        for (int i = 0; i < l.weights.size(); ++i) {
            for (int j = 0; j < l.weights[i].size(); j++) {
                l.weights[i][j] -= lr * grad[i][j];
            }
        }
    }
}

float Train::rand_float(float x) {
    return x * (float)rand() / (float)RAND_MAX;
}

std::vector<Layer> Train::loop() {
    NeuralNetwork model(conf::topology);
    for (int i = 0; i < conf::epochs; ++i) {
        std::vector<float> forward(std::array<float, 2> & activations, Layer & l);
        optimizer(model, conf::lr);
    }
    std::cout << std::endl;
    return model.get_layers();
}
