#pragma once

#include "Header.h"
#include "../math/rng.h"

RENDERING_BEGIN

template <typename T>
void shuffle(T* samp, int count, int nDimensions, RNG& rng) {
    for (int i = 0; i < count; ++i) {
        // 随机选择一个i右侧的索引
        int other = i + rng.uniformUInt32(count - i);
        for (int j = 0; j < nDimensions; ++j) {
            std::swap(samp[nDimensions * i + j], samp[nDimensions * other + j]);
        }
    }
}

RENDERING_END