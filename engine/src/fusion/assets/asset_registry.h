#pragma once

#include "fusion/assets/asset.h"
#include "fusion/filesystem/file_watcher.h"

namespace fe {
    /**
     * @brief Module used for managing assets.
     */
    class FUSION_API AssetRegistry : public Module::Registrar<AssetRegistry> {
    public:
        AssetRegistry() = default;
        ~AssetRegistry() override;

        template<typename T, typename = std::enable_if_t<std::is_base_of_v<Asset, T>>>
        std::shared_ptr<T> get(const fs::path& filepath) const {
            if (auto it = assets.find(type_id<T>); it != assets.end()) {
                if (auto it1 = it->second.find(filepath); it1 != it->second.end()) {
                    return std::dynamic_pointer_cast<T>(it1->second);
                }
            }
            return nullptr;
        }

        template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<Asset, T>>>
        std::shared_ptr<T> load(const fs::path& path, Args... args) {
            if (auto ref = get<T>(path))
                return ref;
            auto it = assets[type_id<T>].emplace(path, std::make_shared<T>(path, std::forward<Args>(args)...));
            //std::thread t(&Asset::loadResource, it.first->second.get());
            //t.detach();
            it.first->second->loadResource();
            return std::dynamic_pointer_cast<T>(it.first->second);
        }

        template<typename T, typename = std::enable_if_t<std::is_base_of_v<Asset, T>>>
        std::unordered_map<fs::path, std::shared_ptr<Asset>, PathHash>& getAssets() { return assets[type_id<T>]; }
        const std::unordered_map<type_index, std::unordered_map<fs::path, std::shared_ptr<Asset>, PathHash>>& getAllAssets() const { return assets; }

        void releaseAll();

    private:
        //void onUpdate() override;
        //void onFileChanged(const fs::path& path, FileStatus status);

        std::unordered_map<type_index, std::unordered_map<fs::path, std::shared_ptr<Asset>, PathHash>> assets;

        //std::unique_ptr<FileWatcher> fileWatcher;
    };
}
