#include "gli_toolbox.hpp"

#include "fusion/filesystem/file_system.hpp"

#include <gli/gli/gli.hpp>

using namespace fe;

void GliToolbox::Load(Bitmap& bitmap, const fs::path& filepath) {
    std::unique_ptr<gli::texture> texture;
    FileSystem::Read(filepath, [&texture](const uint8_t* data, size_t size) {
        texture = std::make_unique<gli::texture>(gli::load(reinterpret_cast<const char*>(data), size));
    });

    const gli::texture& tex = *texture;
    if (tex.empty()) {
        LOG_ERROR << "Failed to load bitmap file: \"" << filepath << "\"";
        return;
    }

    auto pixels = std::make_unique<uint8_t[]>(tex.size());
    std::memcpy(pixels.get(), tex.data(), tex.size());

    bitmap.data = std::move(pixels);
    bitmap.size = tex.extent();
    bitmap.components = static_cast<uint8_t>(component_count(tex.format()));
    bitmap.hdr = false;
}

void GliToolbox::Write(const Bitmap& bitmap, const fs::path& filepath) {
    throw std::runtime_error("Error: gli::write not exist!");
}
