#include "asset_registry.hpp"

#include "fusion/filesystem/virtual_file_system.hpp"

using namespace fe;

void AssetRegistry::add(const std::shared_ptr<Asset>& asset, const fs::path& filepath) {
    assets[asset->getType()].emplace(filepath, asset);
}

void AssetRegistry::remove(const std::shared_ptr<Asset>& asset) {
    if (auto it = assets.find(asset->getType()); it != assets.end()) {
        if (auto it1 = it->second.find(asset->getPath()); it1 != it->second.end()) {
            it->second.erase(it1);
            if (it->second.empty())
                assets.erase(it);
        }
    }
}

void AssetRegistry::onUpdate() {
    /*if (fileWatcher) {
        if (fileWatcher->getWatchPath() != VirtualFileSystem::Get()->resolvePhysicalPath("Assets")) {
            fileWatcher.reset();
            return;
        }
    } else {
        fs::path assetPath{ VirtualFileSystem::Get()->resolvePhysicalPath("Assets") };
        if (!assetPath.empty()) {
            fileWatcher = std::make_unique<FileWatcher>(assetPath, std::chrono::milliseconds(1000));
            fileWatcher->start([](const fs::path& path, FileStatus status) {
                // Process only regular files, all other file types are ignored
                if (!fs::is_regular_file(path)) {
                    return;
                }

                switch(status) {
                    case FileStatus::Created:
                        LOG_DEBUG << "File created: " << path;
                        break;
                    case FileStatus::Modified:
                        LOG_DEBUG << "File modified: " << path;
                        break;
                    case FileStatus::Erased:
                        LOG_DEBUG << "File erased: " << path;
                        break;
                    default:
                        LOG_DEBUG << "Error! Unknown file status.\n";
                }
            });
        }
    }*/


    /*if (elapsedPurge.getElapsed() != 0) {
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
    }*/
}
