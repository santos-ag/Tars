#pragma once
#include "Layer.hpp"
#include "NeuralNetwork.hpp"
#include "Types.hpp"
#include <array>
#include <vector>

class Train {
  public:
    Train(NeuralNetwork *model);

    NeuralNetwork *model{nullptr};

    f32 cost();

    void optimizer(f32 lr);

    v<Layer> loop();

    m<f32> bgd(Layer &l);
};
