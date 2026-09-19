#include "Math.hpp"

f32 sigmoidf(float x) {
    return 1.0f / (1.0f + exp(-x));
}

f32 ReLU(float x) {
    return (x > 0.0) ? x : 0.0;
}

f32 rand_float(float x) {
    return x * (f32)rand() / (float)RAND_MAX;
}
