#include "Train.hpp"
#include "Layer.hpp"
#include "NeuralNetwork.hpp"
#include "Train_conf.hpp"
#include <iostream>

Train::Train(NeuralNetwork *model) : model(model) {
}

f32 Train::cost() {

    f32 result = 0;
    for (i32 i = 0; i < conf::data_tr.size(); ++i) {
        f32 y = conf::data_tr[i].y;
        f32 p = model->forward(conf::data_tr[i].x)[0];
        f32 d = y - p;
        result += d * d;
    }
    return result / conf::data_tr.size();
}

void Train::optimizer(f32 lr) {
    auto &layers = model->get_layers();
    for (auto &l : layers) {
        auto grad = bgd(l);
        for (i32 i = 0; i < l.weights.size(); ++i) {
            for (i32 j = 0; j < l.weights[i].size(); ++j) {
                l.weights[i][j] -= lr * grad[i][j];
            }
        }
        for (i32 j = 0; j < l.bias.size(); j++) {
            f32 original = l.bias[j];
            l.bias[j] = original + conf::h;

            f32 costp = cost();
            l.bias[j] = original - conf::h;

            auto costm = cost();
            l.bias[j] = original;

            f32 gradbias = (costp - costm) / (2.0f * conf::h);
            l.bias[j] -= conf::lr * gradbias;
        }
    }
}

m<f32> Train::bgd(Layer &l) {
    auto &params = l.weights;
    m<f32> derivates(params.size(), v<f32>(params[0].size()));

    for (i32 i = 0; i < derivates.size(); i++) {
        for (i32 j = 0; j < derivates[0].size(); j++) {
            f32 temp = params[i][j];
            params[i][j] = temp + conf::h;

            f32 costp = cost();
            params[i][j] = temp - conf::h;

            f32 costm = cost();
            derivates[i][j] = (costp - costm) / (2 * conf::h);
            params[i][j] = temp;
        }
    }
    return derivates;
}

v<Layer> Train::loop() {

    for (i32 i = 0; i < conf::epochs; ++i) {
        optimizer(conf::lr);
        std::cout << i << " ";
        std::cout << cost() << '\n';
    }
    std::cout << std::endl;
    return model->get_layers();
}
