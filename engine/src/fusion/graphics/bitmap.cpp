#include "bitmap.hpp"

#include "fusion/utils/file.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

using namespace fe;

Bitmap::Bitmap(const std::filesystem::path& filename, int desired_channels) {
    load(filename, desired_channels);
}

Bitmap::Bitmap(const glm::ivec2& size, int channels, bool hdr)
    : width{size.x}
    , height{size.y}
    , channels{channels}
    , hdr{hdr}
    , pixels{std::make_unique<uint8_t[]>(Bitmap::getSize())} {
}

Bitmap::Bitmap(std::unique_ptr<uint8_t[]>&& data, const glm::ivec2& size, int channels, bool hdr)
    : width{size.x}
    , height{size.y}
    , channels{channels}
    , hdr{hdr}
    , pixels{std::move(data)} {
}

void Bitmap::load(const std::filesystem::path& filename, int desired_channels) {
    assert(std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename));

    static const std::vector<std::string> formats = { ".jpeg", ".jpg", ".png", ".bmp", ".hdr", ".psd", ".tga", ".gif", ".pic", ".psd", ".pgm", ".ppm" };

    auto fileExt = filename.extension().string();
    std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);

    if (fileExt) {

    }

    if (fileExt == ".hdr") {
        File::Read(filename, [&](size_t size, const void* data) {
            pixels = std::unique_ptr<uint8_t[]>(reinterpret_cast<uint8_t*>(stbi_loadf_from_memory(reinterpret_cast<const uint8_t*>(data), static_cast<int>(size), &width, &height, &channels, desired_channels)));
        });
        hdr = true;
    } else {
        File::Read(filename, [&](size_t size, const void* data) {
            pixels = std::unique_ptr<uint8_t[]>(stbi_load_from_memory(reinterpret_cast<const uint8_t*>(data), static_cast<int>(size), &width, &height, &channels, desired_channels));
        });
        hdr = false;
    }

    if (!pixels) {
        throw std::runtime_error("Failed to load bitmap file: " + filename.string());
    }

    if (desired_channels > 0) {
        channels = desired_channels;
    }

    this->filename = filename;
}

void Bitmap::write(const std::filesystem::path& filename) const {
    if (auto parent = filename.parent_path(); !parent.empty())
        std::filesystem::create_directories(parent);

    auto fileExt = filename.extension().string();
    std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);

    if (fileExt == ".jpg") {
        stbi_write_jpg(filename.c_str(), width, height, channels, pixels.get(), 8);
    } else if (fileExt == ".bmp") {
        stbi_write_bmp(filename.c_str(), width, height, channels, pixels.get());
    } else if (fileExt == ".png") {
        stbi_write_png(filename.c_str(), width, height, channels, pixels.get(), 0);
    } else if (fileExt == ".tga") {
        stbi_write_tga(filename.c_str(), width, height, channels, pixels.get());
    } else if (fileExt == ".hdr") {
        stbi_write_hdr(filename.c_str(), width, height, channels, reinterpret_cast<float*>(pixels.get()));
    }
}

