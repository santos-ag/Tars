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

float Train::forward(std::array<float, 2> &activations, std::vector<float> &params) {
    float z = params[params.size() - 1];
    for (int i = 0; i < params.size() - 1; ++i) {
        z += params[i] * activations[i];
    }
    return sigmoidf(z);
}

float Train::cost(std::vector<float> &params) {
    float result = 0;
    for (int i = 0; i < conf::data_tr.size(); ++i) {
        float y = conf::data_tr[i].y;

        float p = forward(conf::data_tr[i].x, params);

        float d = y - p;
        result += d * d;
    }
    return result / conf::data_tr.size();
}

std::vector<float> Train::bgd(std::vector<float> params) {
    std::vector<float> derivates(params.size());
    for (int i = 0; i < params.size(); ++i) {
        float temp = params[i];
        params[i] = temp + conf::h;
        float costp = cost(params);
        params[i] = temp - conf::h;
        float costm = cost(params);
        derivates[i] = (costp - costm) / (2 * conf::h);
        params[i] = temp;
    }
    return derivates;
}

void Train::optimizer(NeuralNetwork model, float lr) {
    for (auto &l : model.get_layers()) {

        std::vector<float> grad = bgd(l.weights);

        for (int i = 0; i < l.weights.size(); ++i) {
            l.weights[i] -= lr * grad[i];
        }
    }
}

float Train::rand_float(float x) {
    return x * (float)rand() / (float)RAND_MAX;
}

std::vector<Layer> Train::loop() {
    NeuralNetwork model(conf::topology);
    for (int i = 0; i < conf::epochs; ++i) {

        // std::cout << "epoch: " << i << " w1: " << params[0] << " w1: " << params[1]
        //           << " bias: " << params[2] << " cost: " << cost(params) << std::endl;
        optimizer(model, conf::lr);
    }
    std::cout << std::endl;
    return model.get_layers();
}
