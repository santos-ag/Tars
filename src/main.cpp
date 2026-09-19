#include "./Train.hpp"
#include "Layer.hpp"
#include "NeuralNetwork.hpp"
#include "Train_conf.hpp"
#include "Types.hpp"
#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>
#include <vector>

using std::cout;

int main() {
    srand(time(0));
    NeuralNetwork model(conf::topology);
    Train train(&model);

    v<Layer> res2 = train.loop();

    for (auto l : res2) {
        for (usize i = 0; i < l.out; i++) {
            for (usize j = 0; j < l.in; j++) {
                cout << "w" << j << " " << l.weights[i][j] << " ";
            }
            cout << " bias : " << l.bias[i] << " ";
        }
        cout << std::endl;
    }
}
