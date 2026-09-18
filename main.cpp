#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>
#include <vector>
using namespace std;
const float der = 1e-4f;
vector<float> data_tr;
class Train {
    const int epochs = 10000;
    const float lr = 1e-2;
    float oracle(float x) {
        return 13 * x - 2;
    }

  public:
    float cost(float w, float b) {
        float result = 0;
        for (int i = 0; i < data_tr.size(); ++i) {
            float x = data_tr[i];
            float y = oracle(x);
            float p = w * x + b;
            float d = y - p;
            result += d * d;
            // cout << w << " " << oracle(train_tr[i]) << '\n';
        }
        return result / data_tr.size();
    }
    vector<float> derivate(float w, float b) {
        return {(cost(w + der, b) - cost(w - der, b)) / (2.0f * der),
                (cost(w, b + der) - cost(w, b - der)) / (2.0f * der)};
    }
    vector<float> loop() {
        srand(time(nullptr));
        float w = 10.0f * (float)rand() / RAND_MAX;
        float b = 5.0f * (float)rand() / RAND_MAX;
        for (int i = 0; i < epochs; ++i) {

            cout << " w: " << w << " b: " << b << " cost: " << cost(w, b) << endl;
            vector<float> grad = derivate(w, b);
            w = w - lr * grad[0];
            b = b - lr * grad[1];
        }
        cout << endl;
        return {w, b};
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
