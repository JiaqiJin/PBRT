#pragma once

#include "../core/Header.h"

RENDERING_BEGIN

bool isAbsolutePath(const std::string& filename);

std::string absolutePath(const std::string& filename);

std::string resolveFilename(const std::string& filename);

std::string directoryContaining(const std::string& filename);

void setSearchDirectory(const std::string& dirname);

inline bool hasExtension(const std::string& value, const std::string& ending) {
    if (ending.size() > value.size()) {
        return false;
    }
    return std::equal(
        ending.rbegin(), ending.rend(), value.rbegin(),
        [](char a, char b) { return std::tolower(a) == std::tolower(b); });
}

RENDERING_END