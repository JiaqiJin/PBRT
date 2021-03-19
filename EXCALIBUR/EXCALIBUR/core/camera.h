#pragma once

#include "Header.h"

RENDERING_BEGIN

// 相机样本
struct CameraSample {
    // 在胶片上采样的随机数
    Point2f pFilm;
    // 在透镜上采样的随机数
    Point2f pLens;
    // 采样时间
    Float time;
    // 在filter上的权重
    Float weight;
};

RENDERING_END