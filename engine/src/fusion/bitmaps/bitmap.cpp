#include "bitmap.h"

#include "fusion/filesystem/file_system.h"

using namespace fe;

Bitmap::Bitmap(const fs::path& filepath) {
    load(filepath);
}

Bitmap::Bitmap(const glm::uvec2& size, VkFormat format)
        : size{size}
        , format{format}
        , data{std::make_unique<uint8_t[]>(size.x * size.y * vku::get_format_params(format).bytes)} {
}

Bitmap::Bitmap(std::unique_ptr<uint8_t[]>&& data, const glm::uvec2& size, VkFormat format)
        : size{size}
        , format{format}
        , data{std::move(data)} {
}

void Bitmap::load(const fs::path& filepath) {
#if FUSION_DEBUG
    auto debugStart = DateTime::Now();
#endif

    std::string extension{ FileSystem::GetExtension(filepath) };
    if (auto it = Registry().find(extension); it != Registry().end()) {
        auto& loadFunc = it->second.first;
        loadFunc(*this, filepath);
        path = filepath;
    } else {
        FE_LOG_ERROR("Unknown file extension format: '{}' for the file: '{}'", extension, filepath);
        return;
    }

#if FUSION_DEBUG
    FE_LOG_DEBUG("Bitmap: '{}' loaded in {}ms", filepath, (DateTime::Now() - debugStart).asMilliseconds<float>());
#endif
}

void Bitmap::write(const fs::path& filepath) const {
#if FUSION_DEBUG
    auto debugStart = DateTime::Now();
#endif
    if (auto parent = filepath.parent_path(); !parent.empty())
        fs::create_directories(parent);

    std::string extension{ FileSystem::GetExtension(filepath) };
    if (auto it = Registry().find(extension); it != Registry().end()) {
        auto& writeFunc = it->second.second;
        writeFunc(*this, filepath);
    } else {
        FE_LOG_ERROR("Unknown file extension format: '{}' for the file: '{}'", extension, filepath);
        return;
    }

#if FUSION_DEBUG
    FE_LOG_DEBUG("Bitmap: '{}' written in {}ms", filepath, (DateTime::Now() - debugStart).asMilliseconds<float>());
#endif
}