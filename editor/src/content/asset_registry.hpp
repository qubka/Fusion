#pragma once

#include "asset.hpp"

namespace fe {
    class AssetRegistry {
    public:
        void reset(const std::string& dir);
        void clear();

    private:

        void registerAllAssets(const std::string& filename);
        void registerAsset(const std::string& filename);


        std::unordered_map<std::string, AssetInfo> assets;
    };
}
