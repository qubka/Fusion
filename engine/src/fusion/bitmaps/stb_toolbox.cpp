#include "stb_toolbox.hpp"

#include "fusion/filesystem/file_system.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

using namespace fe;

void StbToolbox::Load(Bitmap& bitmap, const fs::path& filepath) {
    std::unique_ptr<uint8_t[]> pixels;
    int width, height, channels;
    int desired_channels = STBI_rgb_alpha;
    bool hdr = false;

    std::string extension{ FileSystem::GetExtension(filepath) };
    if (extension == ".hdr") {
        FileSystem::Read(filepath, [&](const uint8_t* data, size_t size) {
            pixels = std::unique_ptr<uint8_t[]>(reinterpret_cast<uint8_t*>(stbi_loadf_from_memory(data, static_cast<int>(size), &width, &height, &channels, desired_channels)));
        });
        hdr = true;
    } else {
        FileSystem::Read(filepath, [&](const uint8_t* data, size_t size) {
            pixels = std::unique_ptr<uint8_t[]>(stbi_load_from_memory(data, static_cast<int>(size), &width, &height, &channels, desired_channels));
        });
    }

    if (!pixels || width == 0 || height == 0) {
        LOG_ERROR << "Failed to load bitmap file: " << filepath;
        return;
    }

    bitmap.data = std::move(pixels);
    bitmap.size = { width, height };
    bitmap.components = static_cast<uint8_t>(channels);
    bitmap.hdr = hdr;
}

void StbToolbox::Write(const Bitmap& bitmap, const fs::path& filepath) {
    std::string extension{ FileSystem::GetExtension(filepath) };
    if (extension == ".jpg" || extension == ".jpeg") {
        stbi_write_jpg(filepath.string().c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(bitmap.getComponents()), bitmap.getData<uint8_t>(), 8);
    } else if (extension == ".bmp") {
        stbi_write_bmp(filepath.string().c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(bitmap.getComponents()), bitmap.getData<uint8_t>());
    } else if (extension == ".png") {
        stbi_write_png(filepath.string().c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(bitmap.getComponents()), bitmap.getData<uint8_t>(), 0);
    } else if (extension == ".tga") {
        stbi_write_tga(filepath.string().c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(bitmap.getComponents()), bitmap.getData<uint8_t>());
    } else if (extension == ".hdr") {
        stbi_write_hdr(filepath.string().c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(bitmap.getComponents()), bitmap.getData<float>());
    } else {
        LOG_ERROR << "Unknown extension format: " << extension << " to write data in!";
    }
}
