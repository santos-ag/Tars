#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

class Train {
  public:
    float cost(std::vector<float> &params);

    std::vector<float> bgd(std::vector<float> params);

    void optimizer(std::vector<float> &params, float lr);

    std::vector<float> loop();

  private:
    const int epochs{5000};

    const float h = 1e-4f;

    std::vector<std::array<float, 2>> data_tr;

    const float lr{1e1};

    float oracle(std::array<float, 2> &activations);

    float forward(std::array<float, 2> &activations, std::vector<float> &params);

    float sigmoidf(float x);

    float rfloat(float x);
};
