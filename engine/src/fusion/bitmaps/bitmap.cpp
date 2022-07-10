#include "bitmap.hpp"

#include "fusion/filesystem/file_system.hpp"
#include "fusion/utils/string.hpp"
#include "fusion/utils/date_time.hpp"

using namespace fe;

Bitmap::Bitmap(const fs::path& filename) {
    load(filename);
}

Bitmap::Bitmap(const glm::uvec2& size, BitmapChannels channels, bool hdr)
    : size{size}
    , channels{channels}
    , hdr{hdr}
    , data{std::make_unique<uint8_t[]>(CalculateLength(size, channels, hdr))} {
}

Bitmap::Bitmap(std::unique_ptr<uint8_t[]>&& data, const glm::uvec2& size, BitmapChannels channels, bool hdr)
    : size{size}
    , channels{channels}
    , hdr{hdr}
    , data{std::move(data)} {
}

void Bitmap::load(const fs::path& filename) {
#if FUSION_DEBUG
    auto debugStart = DateTime::Now();
#endif
    auto fileExt = FileSystem::GetExtension(filename);
    if (auto it = Registry().find(fileExt); it != Registry().end()) {
        it->second.first(*this, filename);
        this->filename = filename;
    } else {
        LOG_ERROR << "Unknown file extension format: " << std::quoted(fileExt) << " for the file: " << filename.string();
        return;
    }

#if FUSION_DEBUG
    LOG_DEBUG << "Bitmap: " << filename << " loaded in " << (DateTime::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif
}

void Bitmap::write(const fs::path& filename) const {
#if FUSION_DEBUG
    auto debugStart = DateTime::Now();
#endif
    if (auto parent = filename.parent_path(); !parent.empty())
        fs::create_directories(parent);

    auto fileExt = FileSystem::GetExtension(filename);
    if (auto it = Registry().find(fileExt); it != Registry().end()) {
        it->second.second(*this, filename);
    } else {
        LOG_ERROR << "Unknown file extension format: " << std::quoted(fileExt) << " for the file: " << filename.string();
        return;
    }

#if FUSION_DEBUG
    LOG_DEBUG << "Bitmap: " << filename << " written in " << (DateTime::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif
}

size_t Bitmap::CalculateLength(const glm::uvec2& size, BitmapChannels channels, bool hdr) {
    return size.x * size.y * static_cast<uint8_t>(channels) * (hdr ? sizeof(float) : sizeof(uint8_t));
}
