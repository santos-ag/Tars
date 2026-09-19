#pragma once
#include "Layer.hpp"
#include "Math.hpp"
#include <vector>

class NeuralNetwork {
  public:
    explicit NeuralNetwork(const std::vector<int> &topology);

    float forward(std::vector<float> activations);

    const std::vector<Layer> &get_layers() const;

    std::vector<Layer> &get_layers();

  private:
    std::vector<Layer> layers;
};
