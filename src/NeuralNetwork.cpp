#include "NeuralNetwork.hpp"
#include "Layer.hpp"
#include "Math.hpp"
#include <utility>
#include <vector>

NeuralNetwork::NeuralNetwork(const std::vector<int> &topology) {
    for (int i = 0; i < topology.size() - 1; i++) {
        layers.emplace_back(topology[i], topology[i + 1]);
    }
}

float NeuralNetwork::forward(std::vector<float> activations) {
    std::vector<float> current = activations;

    for (auto &l : layers) {
        const int total_out_neurons = l.bias.size();
        std::vector<float> next(l.out, 0);

        for (int i = 0; i < total_out_neurons; i++) {
            float z{l.bias[i]};
            const int total_weights = l.weights.size();

            for (int j = 0; j < total_weights; j++) {
                float w = l.weights[j][i];
                z += w * current[j];
            }
            next[i] = z;
        }
        l.activations = next;
        current = std::move(next);
    }
    return current.front();
}

const std::vector<Layer> &NeuralNetwork::get_layers() const {
    return layers;
}

std::vector<Layer> &NeuralNetwork::get_layers() {
    return layers;
}
