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
    srand(time(0));
    NeuralNetwork model(conf::topology);
    Train train(&model);

    vector<Layer> res2 = train.loop();
    for (auto l : res2) {

        int sz1 = l.weights.size();
        for (int i = 0; i < sz1; i++) {

            int sz2 = l.weights[i].size();
            for (int j = 0; j < sz2; j++) {
                std::cout << l.weights[i][j] << '\n';
            }
            std::cout << l.bias[i] << ' ';
        }
    }

    // cout << " w1: " << res[0] << " w1: " << res[1] << " bias: " << res[2]
    //      << " cost: " << train.cost(res2[0]) << endl;
}
