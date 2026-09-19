#pragma once
#include "Layer.hpp"
#include "Math.hpp"
#include "Types.hpp"
#include <vector>
class NeuralNetwork {
  public:
    explicit NeuralNetwork(const v<int> &topology);

    v<float> forward(v<float> activations);

    const v<Layer> &get_layers() const;

    v<Layer> &get_layers();

  private:
    v<Layer> layers;
};
