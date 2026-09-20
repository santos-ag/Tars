#pragma once
#include "Types.hpp"

struct MyColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

namespace ColorPalletes {
inline MyColor GetWeightColor(f32 weight, f32 maxWeight, f32 minWeight) {
    f32 norm = (weight > 0) ? weight / maxWeight : weight / minWeight;
    MyColor color = {0, 0, 0, 255};

    if (norm > 0) {
        f32 t = -norm;
        unsigned char gb = (unsigned char)(255.0f * (1.0f - t));
        color = {255, gb, gb, 255};
    } else {
        f32 t = norm;
        unsigned char gb = (unsigned char)(255.0f * (1.0f - t));
        color = {255, gb, gb, 255};
    }

    return color;
};

inline MyColor GetNeuronColor(f32 activation, f32 maxActivation) {
    MyColor color = {0, 0, 0, 0};
    f32 norm = activation / maxActivation;
    unsigned char lum = (unsigned char)(255.0f * norm);
    return color = {255, 255, 255, lum};
};
} // namespace ColorPalletes
