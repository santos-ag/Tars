#pragma once
#include "Layer.hpp"
#include "NeuralNetwork.hpp"
#include "Types.hpp"
#include <array>
#include <vector>
namespace config {
inline i32 epochs = 1000;
inline f32 lr = 1e-2;
inline f32 h = 1e-4;
}; // namespace config

class Train {
  public:
    Train(NeuralNetwork *model);

    NeuralNetwork *model{nullptr};

    f32 cost();

    void optimizer(f32 lr);

    v<Layer> loop();

    m<f32> bgd(Layer &l);
};
