#pragma once

#include <cstddef>
#include <string>
#include <vector>

class SpinAnimationService {
public:
    std::vector<std::string> generateFrames(size_t frameCount, size_t width) const;
};
