#include "gli_toolbox.h"

#include "fusion/filesystem/file_system.h"

#include <gli/gli.hpp>

using namespace fe;

void GliToolbox::Load(Bitmap& bitmap, const fs::path& filepath) {
    gli::texture texture;
    FileSystem::ReadBytes(filepath, [&texture](gsl::span<const uint8_t> buffer) {
        texture = gli::load(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    });

    if (texture.empty()) {
        FE_LOG_ERROR("Failed to load bitmap file: '{}'", filepath);
        return;
    }

    auto pixels = std::make_unique<uint8_t[]>(texture.size());
    std::memcpy(pixels.get(), texture.data(), texture.size());

    bitmap.data = std::move(pixels);
    bitmap.size = texture.extent();
    bitmap.format = vku::convert_format(texture.format());
}

void GliToolbox::Write(const Bitmap& bitmap, const fs::path& filepath) {
    throw std::runtime_error("Error: gli::write not exist!");
}
