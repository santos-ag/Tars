#pragma once
#include "Layer.hpp"
#include "Math.hpp"
#include "Types.hpp"
#include <vector>
class NeuralNetwork {
  public:
    explicit NeuralNetwork(const v<i32> &topology);

    v<f32> forward(v<f32> activations);

    const v<Layer> &get_layers() const;

    v<Layer> &get_layers();

  private:
    v<Layer> layers;
};
