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

using std::cin;
using std::cout;
using std::endl;

int main() {
    cout << "Quer ver a evoluçao do loss?(1/0)";
    bool debug_mode;
    cin >> debug_mode;

    NeuralNetwork model(conf::topology);
    Train train(&model);
    f32 initial_error = train.cost();
    cout << "\nmodelo inicial:" << endl << endl;
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
    v<Layer> result;
    long elapse_time;
    if (debug_mode) {
        result = train.loop(debug_mode);
    } else {
        using Clock = std::chrono::steady_clock;
        auto initial_time = Clock::now();
        result = train.loop(0);
        auto final_time = Clock::now();
        elapse_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(final_time - initial_time)
                .count();
    }
    cout << "\nmodelo final:" << endl << endl;
    for (auto l : result) {
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
    if (!debug_mode) {
        cout << "Tempo total de treino: " << elapse_time << "ms" << endl;
    }
}
