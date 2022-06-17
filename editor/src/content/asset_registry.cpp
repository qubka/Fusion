#include "asset_registry.hpp"

using namespace fe;

void AssetRegistry::registerAllAssets(const std::string& filename) {
    assert(std::filesystem::exists(filename));
    for (const auto& file : fs::recursive_walk(filename)) {
        if (!is_directory(file)) {
            registerAsset(file);
        }
    }
}

void AssetRegistry::registerAsset(const std::string& filename) {
    assert(std::filesystem::exists(filename));
    auto ftime = std::filesystem::last_write_time(filename);

    if (auto it { assets.find(filename) }; it == assets.end() || it->second.importDate) {
        AssetInfo info;
        if (asset::load(filename, info)) {
            assets[filename] = info;
        }

    }
}

void AssetRegistry::clear() {
    assets.clear();
    //asset.clear();
}

void AssetRegistry::reset(const std::string& dir) {
    clear();
    assert(std::filesystem::exists(dir) && std::filesystem::is_directory(dir));
    registerAllAssets(dir);
}
