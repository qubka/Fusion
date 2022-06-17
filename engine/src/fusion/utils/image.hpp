#pragma once

namespace stb {
    struct Image {
        uint8_t* pixels;
        int width;
        int height;
        int channels;

        Image() = delete;
        explicit Image(const std::string& filename, int desiredChannels = 0, bool flip = false);
        ~Image();
    };
}
