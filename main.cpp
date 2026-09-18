#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>
#include <vector>
using namespace std;
const float h = 1e-4f;
vector<float> data_tr;
class Train {
    const int epochs = 10000;
    const float lr = 1e-2;
    float oracle(float x) {
        return 13 * x - 2;
    }

  public:
    float cost(vector<float> &params) {
        float result = 0;
        for (int i = 0; i < data_tr.size(); ++i) {
            float x = data_tr[i];
            float y = oracle(x);
            float p = params[0] * x + params[1];
            float d = y - p;
            result += d * d;
        }
        return result / data_tr.size();
    }
    vector<float> optimizer(vector<float> params) {
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
    vector<float> loop() {
        srand(time(nullptr));
        vector<float> params = {10.0f * (float)rand() / RAND_MAX, 5.0f * (float)rand() / RAND_MAX};
        for (int i = 0; i < epochs; ++i) {

            cout << " w: " << params[0] << " b: " << params[1] << " cost: " << cost(params) << endl;
            vector<float> grad = optimizer(params);
            params[0] = params[0] - lr * grad[0];
            params[1] = params[1] - lr * grad[1];
        }
        cout << endl;
        return params;
    }
};

int main() {
    for (int i = -100; i <= 100; ++i) {
        data_tr.push_back(i * 0.01);
    }
    Train train;
    vector<float> res = train.loop();
    cout << "w: " << res[0] << " b: " << res[1] << endl;
}
