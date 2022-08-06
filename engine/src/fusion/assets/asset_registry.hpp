#pragma once

#include "fusion/assets/asset.hpp"
#include "fusion/filesystem/file_watcher.hpp"

namespace fe {
    /**
     * @brief Module used for managing assets.
     */
    class AssetRegistry : public Module::Registrar<AssetRegistry> {
    public:
        AssetRegistry() = default;
        ~AssetRegistry() override;

        template<typename T>
        std::shared_ptr<T> get(const fs::path& filepath) const {
            if (auto it = assets.find(type_id<T>); it != assets.end()) {
                if (auto it1 = it->second.find(filepath); it1 != it->second.end()) {
                    return std::dynamic_pointer_cast<T>(it1->second);
                }
            }
            //LOG_ERROR << "Asset not found: \"" << filepath.string() << "\"";
            return nullptr;
        }

        template<typename T, typename... Args>
        std::shared_ptr<T> get_or_emplace(const fs::path& path, Args... args) {
            if (auto asset = get<T>(path))
                return asset;
            auto it = assets[type_id<T>].emplace(path, std::make_shared<T>(std::forward<Args>(args)...));
            return std::dynamic_pointer_cast<T>(it.first->second);
        }

        void add(const std::shared_ptr<Asset>& asset, const fs::path& filepath = "") {
            assets[asset->getType()][filepath.empty() ? asset->getPath() : filepath] = asset;
        }
        void remove(const std::shared_ptr<Asset>& asset);

        template<typename T>
        std::unordered_map<fs::path, std::shared_ptr<Asset>>& getAssets() { return assets[type_id<T>]; }
        const std::unordered_map<type_index, std::unordered_map<fs::path, std::shared_ptr<Asset>>>& getAllAssets() const { return assets; }

        /**
         * Gets the resource loader thread pool.
         * @return The resource loader thread pool.
         */
        //ThreadPool& getThreadPool() { return threadPool; }

    private:
        void onUpdate() override;
        void onFileChanged(const fs::path& path, FileStatus status);

        std::unordered_map<type_index, std::unordered_map<fs::path, std::shared_ptr<Asset>>> assets;

        std::unique_ptr<FileWatcher> fileWatcher;

        //ElapsedTime elapsedPurge;
        //ThreadPool threadPool;
    };
}
