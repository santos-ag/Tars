#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>
#include <vector>
using namespace std;
const float h = 1e-4f;
vector<array<float, 2>> data_tr;
class Train {
    const int epochs = 5000;
    const float lr = 1e1;
    float oracle(array<float, 2> activations) {
        return (activations[0] || activations[1]) ? 1.0f : 0.0f;
    }
    float sigmoidf(float x) {
        return 1.0f / (1.0f + exp(-x));
    }
    float forward(array<float, 2> &activations, vector<float> &params) {
        float z = params[params.size() - 1];
        for (int i = 0; i < params.size() - 1; ++i) {
            z += params[i] * activations[i];
        }
        return sigmoidf(z);
    }

  public:
    float cost(vector<float> &params) {
        float result = 0;
        for (int i = 0; i < data_tr.size(); ++i) {
            float y = oracle(data_tr[i]);

            float p = forward(data_tr[i], params);

            float d = y - p;
            result += d * d;
        }
        return result / data_tr.size();
    }
    vector<float> bgd(vector<float> params) {
        vector<float> derivates(params.size());
        for (int i = 0; i < params.size(); ++i) {
            float temp = params[i];
            params[i] = temp + h;
            float costp = cost(params);
            params[i] = temp - h;
            float costm = cost(params);
            derivates[i] = (costp - costm) / (2 * h);
            params[i] = temp;
        }
        return derivates;
    }
    void optimizer(vector<float> &params, float lr) {
        vector<float> grad = bgd(params);

        for (int i = 0; i < params.size(); ++i) {
            params[i] -= lr * grad[i];
        }
    }
    float rfloat(float x) {
        return x * (float)rand() / (float)RAND_MAX;
    }
    vector<float> loop() {
        srand(time(nullptr));
        vector<float> params = {rfloat(2) - 1, rfloat(2) - 1, rfloat(2) - 1};
        for (int i = 0; i < epochs; ++i) {

            cout << "epoch: " << i << " w1: " << params[0] << " w1: " << params[1]
                 << " bias: " << params[2] << " cost: " << cost(params) << endl;
            optimizer(params, lr);
        }
        cout << endl;
        return params;
    }
};

int main() {
    data_tr = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    Train train;
    vector<float> res = train.loop();
    cout << " w1: " << res[0] << " w1: " << res[1] << " bias: " << res[2]
         << " cost: " << train.cost(res) << endl;
}
