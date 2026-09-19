#include "Train.hpp"
#include "Layer.hpp"
#include "NeuralNetwork.hpp"
#include "Train_conf.hpp"
#include <iostream>

Train::Train(NeuralNetwork *model) : model(model) {
}

float Train::cost() {

    float result = 0;
    for (int i = 0; i < conf::data_tr.size(); ++i) {
        float y = conf::data_tr[i].y;
        float p = model->forward(conf::data_tr[i].x)[0];
        float d = y - p;
        result += d * d;
    }
    return result / conf::data_tr.size();
}

void Train::optimizer(float lr) {
    auto &layers = model->get_layers();
    for (auto &l : layers) {
        auto grad = bgd(l);
        for (int i = 0; i < l.weights.size(); ++i) {
            for (int j = 0; j < l.weights[i].size(); ++j) {
                l.weights[i][j] -= lr * grad[i][j];
            }
        }
        for (int j = 0; j < l.bias.size(); j++) {
            float original = l.bias[j];
            l.bias[j] = original + conf::h;

            float costp = cost();
            l.bias[j] = original - conf::h;

            auto costm = cost();
            l.bias[j] = original;

            float gradbias = (costp - costm) / (2.0f * conf::h);
            l.bias[j] -= conf::lr * gradbias;
        }
    }
}

m<float> Train::bgd(Layer &l) {
    auto &params = l.weights;
    m<float> derivates(params.size(), v<float>(params[0].size()));

    for (int i = 0; i < derivates.size(); i++) {
        for (int j = 0; j < derivates[0].size(); j++) {
            float temp = params[i][j];
            params[i][j] = temp + conf::h;

            float costp = cost();
            params[i][j] = temp - conf::h;

            float costm = cost();
            derivates[i][j] = (costp - costm) / (2 * conf::h);
            params[i][j] = temp;
        }
    }
    return derivates;
}

v<Layer> Train::loop() {

    for (int i = 0; i < conf::epochs; ++i) {
        optimizer(conf::lr);
        std::cout << i << " ";
        std::cout << cost() << '\n';
    }
    std::cout << std::endl;
    return model->get_layers();
}
