#pragma once
#include "Layer.hpp"
#include "NeuralNetwork.hpp"
#include "Types.hpp"
#include <array>
#include <vector>
namespace config {
inline int epochs = 1000;
inline float lr = 1e-2;
inline float h = 1e-4;
}; // namespace config

class Train {
  public:
    Train(NeuralNetwork *model);

    NeuralNetwork *model{nullptr};

    float cost();

    void optimizer(float lr);

    v<Layer> loop();

    v<v<float>> bgd(Layer &l);
};
