#include "NeuralNetwork.hpp"
#include "Layer.hpp"

NeuralNetwork::NeuralNetwork(const std::vector<int> &topology) {
    for (int i = 0; i < topology.size() - 1; i++) {
        layers.emplace_back(topology[i], topology[i + 1]);
    }
}

float NeuralNetwork::forward(std::vector<float> &activations) {
    float z{0};
    for (auto &l : layers) {
        const int total_out_neurons = l.bias.size();
        for (int i = 0; i < total_out_neurons; i++) {
            z = l.bias[i];

            const int total_weights = l.weights.size();
            for (int j = 0; j < total_weights; j++) {
                float w = l.weights[i][j];
                z += w * activations[j];
            }
        }
    }
    return sigmoidf(z);
}

const std::vector<Layer> &NeuralNetwork::get_layers() const {
    return layers;
}

std::vector<Layer> &NeuralNetwork::get_layers() {
    return layers;
}
