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

using std::cin;
using std::cout;

int main() {
    srand(time(0));
    NeuralNetwork model(conf::topology);
    Train train(&model);

    v<Layer> res2 = train.loop();

    for (auto l : res2) {

        i32 sz1 = l.weights.size();
        for (i32 i = 0; i < sz1; i++) {

            i32 sz2 = l.weights[i].size();
            for (i32 j = 0; j < sz2; j++) {
                cout << l.weights[i][j] << " ";
            }

            cout << "  " << l.bias[i] << '\n';
            cout << std::endl;
        }
    }
}
