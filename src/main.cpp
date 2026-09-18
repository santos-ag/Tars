#include "./Train.hpp"
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
    Train train;
    vector<int> topology = {1, 1};

    vector<float> res = train.loop();

    cout << " w1: " << res[0] << " w1: " << res[1] << " bias: " << res[2]
         << " cost: " << train.cost(res) << endl;
}
