#pragma once
#include "Layer.hpp"
#include "NeuralNetwork.hpp"
#include <array>
#include <vector>

namespace config {
inline int epochs = 10000;
inline float lr = 1e-3;
inline float h = 1e-4;
}; // namespace config

class Train {
  public:
    Train(NeuralNetwork *model);

    NeuralNetwork *model{nullptr};

    float cost();

    void optimizer(float lr);

    std::vector<Layer> loop();

    std::vector<std::vector<float>> bgd(Layer &l);
};
