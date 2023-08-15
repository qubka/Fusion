#pragma once

#include "asset.h"
#include "asset_database.h"

#include "fusion/filesystem/file_watcher.h"

namespace fe {
    /**
     * @brief Module used for managing assets.
     */
    class FUSION_API AssetRegistry : public Module::Registrar<AssetRegistry> {
    public:
        AssetRegistry();
        ~AssetRegistry() override;

        template<typename T, typename = std::enable_if_t<std::is_base_of_v<Asset, T>>>
        std::shared_ptr<T> get(uuids::uuid uuid) const {
            if (uuid.is_nil())
                return nullptr;
            if (auto it = assets.find(type_id<T>); it != assets.end()) {
                if (auto it1 = it->second.find(uuid); it1 != it->second.end()) {
                    return std::dynamic_pointer_cast<T>(it1->second);
                }
            }
            return nullptr;
        }

        template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<Asset, T>>>
        std::shared_ptr<T> load(uuids::uuid uuid, Args... args) {
            if (uuid.is_nil())
                return nullptr;
            if (auto ref = get<T>(uuid))
                return ref;
            auto it = assets[type_id<T>].emplace(uuid, std::make_shared<T>(uuid, std::forward<Args>(args)...));
            //std::thread t(&Asset::loadResource, it.first->second.get());
            //t.detach();
            it.first->second->load();
            return std::dynamic_pointer_cast<T>(it.first->second);
        }

        const std::unique_ptr<AssetDatabase>& getDatabase() const { return assetDatabase; }

        template<typename T, typename = std::enable_if_t<std::is_base_of_v<Asset, T>>>
        std::unordered_map<uuids::uuid, std::shared_ptr<Asset>>& getAssets() { return assets[type_id<T>]; }
        const std::unordered_map<type_index, std::unordered_map<uuids::uuid, std::shared_ptr<Asset>>>& getAllAssets() const { return assets; }

        //void loadAll(const fs::path path);
        void releaseAll();

    private:
        void onUpdate() override;

#if !FUSION_VIRTUAL_FS

        void onFileChanged(const fs::path& path, FileEvent event);
        void onFileInit(const fs::path& filepath);
        void onFileModified(const fs::path& filepath);
        void onFileErased(const fs::path& filepath);

        std::unique_ptr<FileWatcher> fileWatcher;
#endif
        std::unique_ptr<AssetDatabase> assetDatabase;
        std::unordered_map<type_index, std::unordered_map<uuids::uuid, std::shared_ptr<Asset>>> assets;
    };
}
