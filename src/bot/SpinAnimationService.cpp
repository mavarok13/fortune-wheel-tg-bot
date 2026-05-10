#include "SpinAnimationService.hpp"

#include <algorithm>

std::vector<std::string> SpinAnimationService::generateFrames(size_t frameCount, size_t width) const {
    std::vector<std::string> frames;
    if (frameCount == 0 || width == 0) {
        return frames;
    }

    frames.reserve(frameCount);
    for (size_t frame = 0; frame < frameCount; ++frame) {
        const size_t pos = frame % width;
        std::string line;
        for (size_t i = 0; i < width; ++i) {
            if (i == pos) {
                line += "[I]";
            } else {
                line += "I";
            }
        }
        frames.push_back(std::move(line));
    }

    return frames;
}
