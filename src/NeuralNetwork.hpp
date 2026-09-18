#pragma once
#include "Layer.hpp"
#include <vector>

class NeuralNetwork {
  public:
    explicit NeuralNetwork(const std::vector<int> &topology) {
        for (int i = 0; i < topology.size() - 1; i++) {
            layers.emplace_back(topology[i], topology[i + 1]);
        }
    }

    // Deixa essa porra assim, se for constante a Rede no uso, pega a de baixo, se não, pega a de
    // cima;
    const std::vector<Layer> &get_layers() const {
        return layers;
    }
    std::vector<Layer> &get_layers() {
        return layers;
    }

  private:
    std::vector<Layer> layers;
};
