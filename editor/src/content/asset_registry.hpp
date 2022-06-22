#pragma once

#include "asset.hpp"

namespace fe {
    class AssetRegistry {
    public:
        void reset(const std::string& dir);
        void clear();

    private:

        void registerAllAssets(const std::filesystem::path& filename);
        void registerAsset(const std::filesystem::path& filename);


        std::unordered_map<std::filesystem::path, AssetInfo> assets;
    };
}
