#include "image.hpp"

using namespace stb;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image::Image(const std::string& filename, int desiredChannels, bool flip) {
    assert(std::filesystem::exists(filename));
    stbi_set_flip_vertically_on_load(flip);
    pixels = stbi_load(filename.c_str(), &width, &height, &channels, desiredChannels);
    if (!pixels) {
        std::cerr << "ERROR: Failed to load image: \"" << filename << "\" - " << stbi_failure_reason();
    }
}

Image::~Image() {
    if (pixels)
        stbi_image_free(pixels);
}