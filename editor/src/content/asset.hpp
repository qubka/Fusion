#pragma once

namespace fe {
    enum class AssetType {
        Unknown,
        Material,
        PhysicsMaterial
    };

    struct AssetInfo {
        AssetType type;
        std::string filepath;
        time_t importDate;
        std::string uuid;
        std::vector<uint8_t> hash;
    };

    namespace asset {
        bool save(const std::filesystem::path& filename, const AssetInfo& file);
        bool load(const std::filesystem::path& filename, AssetInfo& outputFile);
    };
}
