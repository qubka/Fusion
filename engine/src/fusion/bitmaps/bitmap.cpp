#include "bitmap.hpp"

#include "fusion/utils/string.hpp"
#include "fusion/utils/time.hpp"

using namespace fe;

Bitmap::Bitmap(const std::filesystem::path& filename) {
    load(filename);
}

Bitmap::Bitmap(const glm::uvec2& size, BitmapChannels channels, bool hdr)
    : size{size}
    , channels{channels}
    , hdr{hdr}
    , data{std::make_unique<uint8_t[]>(Bitmap::getLength())} {
}

Bitmap::Bitmap(std::unique_ptr<uint8_t[]>&& data, const glm::uvec2& size, BitmapChannels channels, bool hdr)
    : size{size}
    , channels{channels}
    , hdr{hdr}
    , data{std::move(data)} {
}

void Bitmap::load(const std::filesystem::path& filename) {
#ifdef FUSION_DEBUG
    auto debugStart = Time::Now();
#endif
    assert(std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename));

    auto fileExt = String::Lowercase(filename.extension().string());
    if (auto it = Registry().find(fileExt); it != Registry().end()) {
        it->second.first(*this, filename);
        this->filename = filename;
    } else {
        LOG_ERROR << "Unknown file extension format: " << std::quoted(fileExt) << " for the file: " << filename.string();
        return;
    }

#ifdef FUSION_DEBUG
    LOG_DEBUG << "Bitmap " << filename << " loaded in " << (Time::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif
}

void Bitmap::write(const std::filesystem::path& filename) const {
#ifdef FUSION_DEBUG
    auto debugStart = Time::Now();
#endif
    assert(std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename));

    if (auto parent = filename.parent_path(); !parent.empty())
        std::filesystem::create_directories(parent);

    auto fileExt = String::Lowercase(filename.extension().string());
    if (auto it = Registry().find(String::Lowercase(fileExt)); it != Registry().end()) {
        it->second.second(*this, filename);
    } else {
        LOG_ERROR << "Unknown file extension format: " << std::quoted(fileExt) << " for the file: " << filename.string();
        return;
    }

#ifdef FUSION_DEBUG
    LOG_DEBUG << "Bitmap " << filename << " written in " << (Time::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif
}
