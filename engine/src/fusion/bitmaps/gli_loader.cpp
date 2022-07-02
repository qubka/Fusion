#include "gli_loader.hpp"

#include "fusion/utils/file.hpp"

#include <gli/gli/gli.hpp>

using namespace fe;

void GliLoader::Load(Bitmap& bitmap, const std::filesystem::path& filename) {
    std::unique_ptr<gli::texture> texture;
    File::Read(filename, [&texture](size_t size, const void* data) {
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

void GliLoader::Write(const Bitmap& bitmap, const std::filesystem::path& filename) {
    throw std::runtime_error("Error: gli::write not exist!");
}
