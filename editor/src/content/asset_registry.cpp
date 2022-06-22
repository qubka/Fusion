#include "asset_registry.hpp"

using namespace fe;

void AssetRegistry::registerAllAssets(const std::filesystem::path& filename) {
    assert(std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename));
    for (const auto& file : Directory::GetFilesRecu(filename)) {
        if (!is_directory(file)) {
            registerAsset(file);
        }
    }
}

void AssetRegistry::registerAsset(const std::filesystem::path& filename) {
    assert(std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename));
    auto ftime = std::filesystem::last_write_time(filename);

    if (auto it = assets.find(filename); it == assets.end() || it->second.importDate) {
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
