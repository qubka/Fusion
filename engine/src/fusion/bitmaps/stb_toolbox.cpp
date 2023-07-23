#include "stb_toolbox.h"

#include "fusion/filesystem/file_system.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

using namespace fe;

void StbToolbox::Load(Bitmap& bitmap, const fs::path& filepath) {
    std::unique_ptr<std::byte[]> pixels;
    int width, height, channels;
    int desired_channels = STBI_rgb_alpha;
    bool hdr = false;

    std::string extension{ FileSystem::GetExtension(filepath) };
    if (extension == ".hdr") {
        FileSystem::ReadBytes(filepath, [&](std::span<const std::byte> buffer) {
            pixels = std::unique_ptr<std::byte[]>(reinterpret_cast<std::byte*>(
                    stbi_loadf_from_memory(reinterpret_cast<const stbi_uc*>(buffer.data()), static_cast<int>(buffer.size()), &width, &height, &channels, desired_channels)));
        });
        hdr = true;
    } else {
        FileSystem::ReadBytes(filepath, [&](std::span<const std::byte> buffer) {
            pixels = std::unique_ptr<std::byte[]>(reinterpret_cast<std::byte*>(
                    stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(buffer.data()), static_cast<int>(buffer.size()), &width, &height, &channels, desired_channels)));
        });
    }

    if (!pixels || width == 0 || height == 0) {
        LOG_ERROR << "Failed to load bitmap file: \"" << filepath << "\"";
        return;
    }

    bitmap.data = std::move(pixels);
    bitmap.size = { width, height };
    int components = glm::max(channels, desired_channels);
    switch (components) {
        case STBI_grey:
            bitmap.format = hdr ? VK_FORMAT_R16_UNORM : VK_FORMAT_R8_UNORM;
            break;
        case STBI_grey_alpha:
            bitmap.format = hdr ? VK_FORMAT_R16G16_UNORM : VK_FORMAT_R8G8_UNORM;
            break;
        case STBI_rgb:
            bitmap.format = hdr ? VK_FORMAT_R16G16B16_UNORM : VK_FORMAT_R8G8B8_UNORM;
            break;
        case STBI_rgb_alpha:
            bitmap.format = hdr ? VK_FORMAT_R16G16B16A16_UNORM : VK_FORMAT_R8G8B8A8_UNORM;
            break;
    }
}

void StbToolbox::Write(const Bitmap& bitmap, const fs::path& filepath) {
    std::string extension{ FileSystem::GetExtension(filepath) };
    if (extension == ".jpg" || extension == ".jpeg") {
        stbi_write_jpg(filepath.string().c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(vku::get_format_params(bitmap.getFormat()).bytes), bitmap.getData<void>(), 8);
    } else if (extension == ".bmp") {
        stbi_write_bmp(filepath.string().c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(vku::get_format_params(bitmap.getFormat()).bytes), bitmap.getData<void>());
    } else if (extension == ".png") {
        stbi_write_png(filepath.string().c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(vku::get_format_params(bitmap.getFormat()).bytes), bitmap.getData<void>(), 0);
    } else if (extension == ".tga") {
        stbi_write_tga(filepath.string().c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(vku::get_format_params(bitmap.getFormat()).bytes), bitmap.getData<void>());
    } else if (extension == ".hdr") {
        stbi_write_hdr(filepath.string().c_str(), bitmap.getWidth(), bitmap.getHeight(), static_cast<int>(vku::get_format_params(bitmap.getFormat()).bytes / sizeof(float)), bitmap.getData<float>());
    } else {
        LOG_ERROR << "Unknown extension format: \"" << extension << "\" to write data in!";
    }
}
