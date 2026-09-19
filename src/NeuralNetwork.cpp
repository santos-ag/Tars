#include "NeuralNetwork.hpp"
#include "Layer.hpp"
#include "Math.hpp"
#include <utility>
#include <vector>

NeuralNetwork::NeuralNetwork(const v<i32> &topology) {
    for (i32 i = 0; i < topology.size() - 1; i++) {
        layers.emplace_back(topology[i], topology[i + 1]);
    }
}

v<f32> NeuralNetwork::forward(v<f32> activations) {
    // saida de uma vira activation da proxima
    for (auto &l : layers) {
        v<f32> next(l.out, 0);
        // para cada neuronio de saida da camada
        for (i32 i = 0; i < l.out; i++) {
            // bias do iesimo neuronio(o que estamos olhando agr)
            f32 z = l.bias[i];
            // para cada peso que entra nesse neuronio
            for (i32 j = 0; j < l.in; j++) {
                // pega o peso em ordem que entram nesse neuronio
                f32 w = l.weights[i][j];
                // ativação vezes o respectivo peso
                z += w * activations[j];
            }
            next[i] = sigmoidf(z);
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
