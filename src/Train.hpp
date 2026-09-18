#pragma once
#include <array>
#include <vector>

namespace config {
inline int epochs = 10000;
inline float lr = 1e-3;
inline float h = 1e-4;
}; // namespace config

class Train {
  public:
    float cost(std::vector<float> &params);

    std::vector<float> bgd(std::vector<float> params);

    void optimizer(std::vector<float> &params, float lr);

    std::vector<float> loop();

  private:
    float forward(std::array<float, 2> &activations, std::vector<float> &params);

    float sigmoidf(float x);

    float ReLU(float x);

    float rand_float(float x);
};
