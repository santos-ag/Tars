#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#pragma once
class Train {
  public:
    float cost(std::vector<float> &params);

    std::vector<float> bgd(std::vector<float> params);

    void optimizer(std::vector<float> &params, float lr);

    std::vector<float> loop();

  private:
    float forward(std::array<float, 2> &activations, std::vector<float> &params);

    float sigmoidf(float x);

    float rfloat(float x);
};
