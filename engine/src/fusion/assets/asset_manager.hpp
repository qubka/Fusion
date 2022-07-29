#pragma once

#include "asset.hpp"

namespace fe {
    /**
     * @brief Module used for managing assets.
     */
    class AssetManager : public Module::Registrar<AssetManager> {
    public:
        AssetManager() = default;
        ~AssetManager() override = default;

        std::shared_ptr<Asset> find(type_index type, const std::string& serialize) const;

        template<typename T>
        std::shared_ptr<T> find(const std::string& data) const {
            if (auto it = assets.find(type_id<T>); it != assets.end()) {
                if (auto it1 = it->second.find(data); it1 != it->second.end()) {
                    return std::dynamic_pointer_cast<T>(it1->second);
                }
            }
            return nullptr;
        }

        template<typename T, typename... Args>
        std::shared_ptr<T> emplace(std::string&& serialize, Args... args) {
            auto it = assets[type_id<T>].emplace(serialize, std::make_shared<T>(std::forward<Args>(args)...));
            return std::dynamic_pointer_cast<T>(it.first->second);
        }

        //void add(const std::shared_ptr<Asset>& asset, const std::string& serialize);
        //void remove(const std::shared_ptr<Asset>& asset);

        template<typename T>
        std::unordered_map<std::string, std::shared_ptr<Asset>>& getAsset() { return assets[type_id<T>]; }
        const std::unordered_map<type_index, std::unordered_map<std::string, std::shared_ptr<Asset>>>& getAssets() const { return assets; }

        /**
         * Gets the resource loader thread pool.
         * @return The resource loader thread pool.
         */
        ThreadPool& getThreadPool() { return threadPool; }

    private:
        void onUpdate() override;

        std::unordered_map<type_index, std::unordered_map<std::string, std::shared_ptr<Asset>>> assets;
        ElapsedTime elapsedPurge;
        ThreadPool threadPool;
    };
}
