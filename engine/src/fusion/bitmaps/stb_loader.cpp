#include "stb_loader.hpp"

#include "fusion/utils/string.hpp"
#include "fusion/utils/file.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

using namespace fe;

void StbLoader::Load(Bitmap& bitmap, const std::filesystem::path& filename) {
    std::unique_ptr<uint8_t[]> pixels;
    int width, height, channels;
    int desired_channels = STBI_rgb_alpha;
    bool hdr = false;

    auto fileExt = String::Uppercase(filename.extension().string());
    if (fileExt == ".hdr") {
        File::Read(filename, [&](size_t size, const void* data) {
            pixels = std::unique_ptr<uint8_t[]>(reinterpret_cast<uint8_t*>(stbi_loadf_from_memory(reinterpret_cast<const uint8_t*>(data), static_cast<int>(size), &width, &height, &channels, desired_channels)));
        });
        hdr = true;
    } else {
        File::Read(filename, [&](size_t size, const void* data) {
            pixels = std::unique_ptr<uint8_t[]>(stbi_load_from_memory(reinterpret_cast<const uint8_t*>(data), static_cast<int>(size), &width, &height, &channels, desired_channels));
        });
    }

    bitmap.data = std::move(pixels);
    bitmap.size = { width, height };
    bitmap.channels = static_cast<BitmapChannels>(channels);
    bitmap.hdr = hdr;

    if (bitmap.isEmpty()) {
        LOG_ERROR << "Failed to load bitmap file: " << filename.string();
        return;
    }

    /*if (desired_channels > 0) {
        channels = desired_channels;
    }*/
}

void StbLoader::Write(const Bitmap& bitmap, const std::filesystem::path& filename) {
    auto fileExt = String::Uppercase(filename.extension().string());
    if (fileExt == ".jpg" || fileExt == ".jpeg") {
        stbi_write_jpg(filename.c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(bitmap.getChannels()), bitmap.getData<uint8_t>(), 8);
    } else if (fileExt == ".bmp") {
        stbi_write_bmp(filename.c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(bitmap.getChannels()), bitmap.getData<uint8_t>());
    } else if (fileExt == ".png") {
        stbi_write_png(filename.c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(bitmap.getChannels()), bitmap.getData<uint8_t>(), 0);
    } else if (fileExt == ".tga") {
        stbi_write_tga(filename.c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(bitmap.getChannels()), bitmap.getData<uint8_t>());
    } else if (fileExt == ".hdr") {
        stbi_write_hdr(filename.c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(bitmap.getChannels()), bitmap.getData<float>());
    } else {
        LOG_ERROR << "Unknown extension format: " << std::quoted(fileExt) << " to write data in!";
    }
}
