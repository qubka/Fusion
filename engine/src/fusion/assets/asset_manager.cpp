#include "asset_manager.hpp"

using namespace fe;

std::shared_ptr<Asset> AssetManager::find(type_index type, const std::string& serialize) const {
    if (auto it = assets.find(type); it != assets.end()) {
        if (auto it1 = it->second.find(serialize); it1 != it->second.end()) {
            return it1->second;
        }
    }
    return nullptr;
}

void AssetManager::add(const std::shared_ptr<Asset>& asset, const std::string& serialize) {
    auto type = asset->getTypeIndex();
    if (find(type, serialize))
        return;

    assets[type].emplace(serialize, asset);
}

void AssetManager::remove(const std::shared_ptr<Asset>& asset) {
    auto type = asset->getTypeIndex();
    auto data = "";

    if (auto it = assets.find(type); it != assets.end()) {
        if (auto it1 = it->second.find(data); it1 != it->second.end()) {
            it->second.erase(it1);
            if (it->second.empty())
                assets.erase(it);
        }
    }
}

void AssetManager::onUpdate() {
    if (elapsedPurge.getElapsed() != 0) {
        for (auto it = assets.begin(); it != assets.end();) {
            for (auto it1 = it->second.begin(); it1 != it->second.end();) {
                if ((*it1).second.use_count() <= 1) {
                    it1 = it->second.erase(it1);
                    continue;
                }
                ++it1;
            }

            if (it->second.empty()) {
                it = assets.erase(it);
                continue;
            }

            ++it;
        }
    }
}
