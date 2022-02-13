#pragma once

namespace Fusion {
    struct FUSION_API Image {
        uint8_t* pixels;
        int width;
        int height;
        int channels;

        Image(const std::string& path, int channels = 4, bool flip = true);
        ~Image();
    };
}