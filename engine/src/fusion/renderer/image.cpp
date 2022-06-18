#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace fe;

std::shared_ptr<Image> Image::fromFile(const std::string& filename, int channels, bool flip) {
    assert(std::fs::exists(filename));

    std::shared_ptr<Image> image{new Image};

    stbi_set_flip_vertically_on_load(flip);

    if (stbi_is_hdr(filename.c_str())) {
        float* pixels = stbi_loadf(filename.c_str(), &image->width_, &image->height_, &image->channels_, channels);
        if (pixels) {
            image->pixels_.reset(reinterpret_cast<unsigned char*>(pixels));
            image->hdr_ = true;
        }
    } else {
        unsigned char* pixels = stbi_load(filename.c_str(), &image->width_, &image->height_, &image->channels_, channels);
        if (pixels) {
            image->pixels_.reset(pixels);
            image->hdr_ = false;
        }
    }

    if (!image->pixels_) {
        throw std::runtime_error("Failed to load image file: " + filename);
    }

    if (channels > 0) {
        image->channels_ = channels;
    }

    return image;
}