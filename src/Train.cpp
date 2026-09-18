#include "Train.hpp"
#include "./train_conf.hpp"
float conf::oracle(std::array<float, 2> &activations) {
    return (activations[0] || activations[1]) ? 1.0f : 0.0f;
}

float Train::sigmoidf(float x) {
    return 1.0f / (1.0f + exp(-x));
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
        float y = conf::oracle(conf::data_tr[i]);

        float p = forward(conf::data_tr[i], params);

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

void Train::optimizer(std::vector<float> &params, float lr) {
    std::vector<float> grad = bgd(params);

    for (int i = 0; i < params.size(); ++i) {
        params[i] -= lr * grad[i];
    }
}

float Train::rfloat(float x) {
    return x * (float)rand() / (float)RAND_MAX;
}

std::vector<float> Train::loop() {
    srand(time(nullptr));
    std::vector<float> params = {rfloat(2) - 1, rfloat(2) - 1, rfloat(2) - 1};
    for (int i = 0; i < conf::epochs; ++i) {

        std::cout << "epoch: " << i << " w1: " << params[0] << " w1: " << params[1]
                  << " bias: " << params[2] << " cost: " << cost(params) << std::endl;
        optimizer(params, conf::lr);
    }
    std::cout << std::endl;
    return params;
}
