#include "./Train.hpp"
#include "Layer.hpp"
#include "NeuralNetwork.hpp"
#include "Train_conf.hpp"
#include "Types.hpp"
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>
#include <vector>

using std::cout;
using std::endl;

int main() {
    using Clock = std::chrono::steady_clock;

    NeuralNetwork model(conf::topology);
    Train train(&model);
    f32 initial_error = train.cost();
    cout << "modelo inicial:" << endl << endl;
    for (auto l : model.get_layers()) {
        for (usize i = 0; i < l.out; i++) {
            for (usize j = 0; j < l.in; j++) {
                cout << "w" << j << " " << l.weights[i][j] << " ";
            }
            cout << " bias : " << l.bias[i] << "; ";
        }
        cout << endl;
    }
    cout << endl;

    auto initial_time = Clock::now();
    v<Layer> res2 = train.loop();
    auto final_time = Clock::now();
    auto elapse_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(final_time - initial_time).count();
    cout << "modelo final:" << endl << endl;
    for (auto l : res2) {
        for (usize i = 0; i < l.out; i++) {
            for (usize j = 0; j < l.in; j++) {
                cout << "w" << j << " " << l.weights[i][j] << " ";
            }
            cout << " bias : " << l.bias[i] << "; ";
        }
        cout << endl;
    }
    cout << endl;
    cout << "Erro da rede:" << endl << initial_error << "-->" << train.cost() << endl;
    cout << "Tempo total de treino: " << elapse_time << "ms" << endl;
}
