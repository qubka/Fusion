#include "asset_registry.hpp"

#include "fusion/core/engine.hpp"

using namespace fe;

#define BIND_FILECHANGE_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

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
        if (fileWatcher->getWatchPath() != Engine::Get()->getApp()->getRootPath()) {
            fileWatcher.reset();
            return;
        }
        fileWatcher->update();
    } else {
        auto& rootPath = Engine::Get()->getApp()->getRootPath();
        if (!rootPath.empty()) {
            fileWatcher = std::make_unique<FileWatcher>(rootPath, BIND_FILECHANGE_FN(onFileChanged));
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

void AssetRegistry::onFileChanged(const fs::path& path, FileStatus status) {
    switch (status) {
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
}
