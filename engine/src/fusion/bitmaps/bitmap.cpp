#include "bitmap.hpp"

#include "fusion/filesystem/file_system.hpp"

using namespace fe;

Bitmap::Bitmap(const fs::path& filepath) {
    load(filepath);
}

Bitmap::Bitmap(const glm::uvec2& size, VkFormat format)
        : size{size}
        , format{format}
        , data{std::make_unique<std::byte[]>(size.x * size.y * vku::get_format_params(format).bytes)} {
}

Bitmap::Bitmap(std::unique_ptr<std::byte[]>&& data, const glm::uvec2& size, VkFormat format)
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
        it->second.first(*this, filepath);
        path = filepath;
    } else {
        LOG_ERROR << "Unknown file extension format: \"" << extension << "\" for the file: \"" << filepath << "\"";
        return;
    }

#if FUSION_DEBUG
    LOG_DEBUG << "Bitmap: \"" << filepath << "\" loaded in " << (DateTime::Now() - debugStart).asMilliseconds<float>() << "ms";
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
        it->second.second(*this, filepath);
    } else {
        LOG_ERROR << "Unknown file extension format: \"" << extension << "\" for the file: \"" << filepath << "\"";
        return;
    }

#if FUSION_DEBUG
    LOG_DEBUG << "Bitmap: \"" << filepath << "\" written in " << (DateTime::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif
}