#include "gli_loader.hpp"

#include "fusion/filesystem/file_system.hpp"

#include <gli/gli/gli.hpp>

using namespace fe;

void GliLoader::Load(Bitmap& bitmap, const fs::path& filename) {
    std::unique_ptr<gli::texture> texture;
    FileSystem::Read(filename, [&texture](const uint8_t* data, size_t size) {
        texture = std::make_unique<gli::texture>(gli::load(reinterpret_cast<const char*>(data), size));
    });

    bitmap.data = std::unique_ptr<uint8_t[]>(texture->data<uint8_t>());
    bitmap.size = texture->extent();
    bitmap.channels = static_cast<BitmapChannels>(block_size(texture->format()));
    bitmap.hdr = false;

    if (bitmap.isEmpty()) {
        LOG_ERROR << "Failed to load bitmap file: " << filename.string();
        return;
    }
}

void GliLoader::Write(const Bitmap& bitmap, const fs::path& filename) {
    throw std::runtime_error("Error: gli::write not exist!");
}
