#include "asset_registry.hpp"
#include "fusion/utils/directory.hpp"

using namespace fe;

void AssetRegistry::registerAllAssets(const std::filesystem::path& filename) {
    assert(std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename));
    for (const auto& file : Directory::GetFiles(filename)) {
        if (!is_directory(file)) {
            registerAsset(file);
        }
    }
}

void AssetRegistry::registerAsset(const std::filesystem::path& filename) {
    assert(std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename));
    auto ftime = std::filesystem::last_write_time(filename);

    auto key = filename.wstring();
    if (auto it = assets.find(key); it == assets.end() || it->second.importDate) {
        AssetInfo info;
        if (asset::load(filename, info)) {
            assets[key] = info;
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