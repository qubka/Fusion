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

        void add(const std::shared_ptr<Asset>& asset, const std::string& serialize);
        void remove(const std::shared_ptr<Asset>& asset);

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
