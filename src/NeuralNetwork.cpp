#include "NeuralNetwork.hpp"
#include "Layer.hpp"
#include "Math.hpp"
#include <utility>
#include <vector>

NeuralNetwork::NeuralNetwork(const v<int> &topology) {
    for (int i = 0; i < topology.size() - 1; i++) {
        layers.emplace_back(topology[i], topology[i + 1]);
    }
}

v<float> NeuralNetwork::forward(v<float> activations) {
    // saida de uma vira activation da proxima
    for (auto &l : layers) {
        v<float> next(l.out, 0);
        // para cada neuronio de saida da camada
        for (int i = 0; i < l.out; i++) {
            // bias do iesimo neuronio(o que estamos olhando agr)
            float z = l.bias[i];
            // para cada peso que entra nesse neuronio
            for (int j = 0; j < l.in; j++) {
                // pega o peso em ordem que entram nesse neuronio
                float w = l.weights[i][j];
                // ativação vezes o respectivo peso
                z += w * activations[j];
            }
            next[i] = z;
        }
        activations = next;
    }
    return activations;
}

const v<Layer> &NeuralNetwork::get_layers() const {
    return layers;
}

v<Layer> &NeuralNetwork::get_layers() {
    return layers;
}
