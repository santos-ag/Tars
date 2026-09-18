#include "./Train.hpp"
#include "Layer.hpp"
#include "NeuralNetwork.hpp"
#include "Train_conf.hpp"
#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>
#include <vector>
using namespace std;

int main() {
    NeuralNetwork model(conf::topology);
    Train train(&model);

    vector<Layer> res2 = train.loop();
    vector<float> res = res2[0].weights[0];
    cout << " w1: " << res[0] << " w1: " << res[1] << " bias: " << res[2]
         << " cost: " << train.cost(res2[0]) << endl;
}
