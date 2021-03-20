#pragma once

#include "../core/Header.h"
#include <fstream>
#include "fileutil.h"

RENDERING_BEGIN

std::unique_ptr<RGBSpectrum[]> readImage(const std::string& name, Point2i* resolution);

void writeImage(const std::string& name, const Float* rgb,
    const AABB2i& outputBounds, const Point2i& totalResolution);

inline nloJson createJsonFromFile(const std::string& fn) {
    std::ifstream fst;
    fst.open(fn.c_str());
    std::stringstream buffer;
    buffer << fst.rdbuf();
    std::string str = buffer.str();
    if (hasExtension(fn, "bson")) {
        return nloJson::from_bson(str);
    }
    else {
        return nloJson::parse(str);
    }
}

RENDERING_END