#include "Math.hpp"

float sigmoidf(float x) {
    return 1.0f / (1.0f + exp(-x));
}

float ReLU(float x) {
    return (x > 0.0) ? x : 0.0;
}

float rand_float(float x) {
    return x * (float)rand() / (float)RAND_MAX;
}
