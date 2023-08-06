#include "gli_toolbox.h"

#include "fusion/filesystem/file_system.h"

#include <gli/gli.hpp>

using namespace fe;

void GliToolbox::Load(Bitmap& bitmap, const fs::path& filepath) {
    std::unique_ptr<gli::texture> texture;
    FileSystem::ReadBytes(filepath, [&texture](gsl::span<const std::byte> buffer) {
        texture = std::make_unique<gli::texture>(gli::load(reinterpret_cast<const char*>(buffer.data()), buffer.size()));
    });

    const gli::texture& tex = *texture;
    if (tex.empty()) {
        FS_LOG_ERROR("Failed to load bitmap file: '{}'", filepath);
        return;
    }

    auto pixels = std::make_unique<std::byte[]>(tex.size());
    std::memcpy(pixels.get(), tex.data(), tex.size());

    bitmap.data = std::move(pixels);
    bitmap.size = tex.extent();
    bitmap.format = vku::convert_format(tex.format());
}

void GliToolbox::Write(const Bitmap& bitmap, const fs::path& filepath) {
    throw std::runtime_error("Error: gli::write not exist!");
}
