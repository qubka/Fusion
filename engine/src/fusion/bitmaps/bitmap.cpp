#include "bitmap.hpp"

#include "fusion/filesystem/file_system.hpp"
#include "fusion/utils/string.hpp"
#include "fusion/utils/date_time.hpp"

using namespace fe;

Bitmap::Bitmap(const fs::path& filepath) {
    load(filepath);
}

Bitmap::Bitmap(const glm::uvec2& size, uint8_t components, bool hdr)
    : size{size}
    , components{components}
    , hdr{hdr}
    , data{std::make_unique<uint8_t[]>(CalculateLength(size, components, hdr))} {
}

Bitmap::Bitmap(std::unique_ptr<uint8_t[]>&& data, const glm::uvec2& size, uint8_t components, bool hdr)
    : size{size}
    , components{components}
    , hdr{hdr}
    , data{std::move(data)} {
}

void Bitmap::load(const fs::path& filepath) {
#if FUSION_DEBUG
    auto debugStart = DateTime::Now();
#endif
    auto extension = FileSystem::GetExtension(filepath);
    if (auto it = Registry().find(extension); it != Registry().end()) {
        it->second.first(*this, filepath);
        filePath = filepath;
    } else {
        LOG_ERROR << "Unknown file extension format: " << extension << " for the file: " << filepath;
        return;
    }

#if FUSION_DEBUG
    LOG_DEBUG << "Bitmap: " << filepath << " loaded in " << (DateTime::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif
}

void Bitmap::write(const fs::path& filepath) const {
#if FUSION_DEBUG
    auto debugStart = DateTime::Now();
#endif
    if (auto parent = filepath.parent_path(); !parent.empty())
        fs::create_directories(parent);

    auto extension = FileSystem::GetExtension(filepath);
    if (auto it = Registry().find(extension); it != Registry().end()) {
        it->second.second(*this, filepath);
    } else {
        LOG_ERROR << "Unknown file extension format: " << extension << " for the file: " << filepath;
        return;
    }

#if FUSION_DEBUG
    LOG_DEBUG << "Bitmap: " << filepath << " written in " << (DateTime::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif
}

size_t Bitmap::CalculateLength(const glm::uvec2& size, uint8_t components, bool hdr) {
    return size.x * size.y * components * (hdr ? sizeof(float) : sizeof(uint8_t));
}
