#pragma once

#include "fusion/scene/system_holder.hpp"

#include <entt/entity/registry.hpp>
#include <cereal/cereal.hpp>

namespace fe {
    class Camera;
    class Scene {
        friend class SceneManager;
    public:
        /**
         * Creates a new scene.
         * @param name The scenes name.
         */
        explicit Scene(std::string name);
        ~Scene() = default;

        /**
         * Called when the scene is created.
         */
        virtual void onCreate();

        /**
         * Run when updating the scene.
         */
        virtual void onUpdate();

        /**
         * Called when scene is being activated, and will begin being rendered/updated.
         */
        virtual void onStart();

        /**
         * Called when scene is being swapped and will no longer be rendered/updated
         */
        virtual void onStop();

        /**
         * Checks whether a System exists or not.
         * @tparam T The System type.
         * @return If the System exists.
         */
        template<typename T>
        bool hasSystem() const {
            return systems.has<T>();
        }

        /**
         * Gets a System.
         * @tparam T The System type.
         * @return The System.
         */
        template<typename T>
        T* getSystem() const {
            return systems.get<T>();
        }

        /**
         * Adds a System.
         * @tparam T The System type.
         * @tparam Args The constructor arg types.
         * @param args The constructor args.
         * @return The System.
         */
        template<typename T, typename... Args>
        void addSystem(Args&&...args) {
            systems.add<T>(std::make_shared<T>(registry, std::forward<Args>(args)...));
        }

        /**
         * Removes a System.
         * @tparam T The System type.
         */
        template<typename T>
        void removeSystem() {
            systems.remove<T>();
        }

        /**
         * Removes all Systems.
         */
        void clearSystems();

        /**
         * Removes all Entities.
         */
        void clearEntities();

        /**
         * Gets the current camera object.
         * @return The current camera.
         */
        Camera* getCamera() const { return camera.get(); }

        /**
         * Sets the current camera to a new camera.
         * @param cam The new camera.
         */
        void setCamera(std::shared_ptr<Camera> cam) { camera = std::move(cam); }

        /**
         * Gets the name of the scene.
         * @return The scene name.
         */
        const std::string& getName() const { return name; }

        /**
         * Sets the name to the scene.
         * @param str The scene name.
         */
        void setName(const std::string& str) { name = str; }

        /**
         * Gets if the scene is in runtime.
         * @return If the scene is in runtime.
         */
        bool isRuntime() { return runtime; }

        entt::entity createEntity(std::string name = "");

        void destroyEntity(entt::entity entity);

        entt::entity duplicateEntity(entt::entity entity);

        /**
         * Gets the scene entity registry.
         * @return Entity registry.
         */
        const entt::registry& getRegistry() const { return registry; }
        entt::registry& getRegistry() { return registry; }

        /**
         * Sets the scene entity registry.
         * @param camera The new entity registry.
         */
        void setRegistry(entt::registry&& reg) { registry = std::move(reg) ; }

        template<typename T>
        auto getAllEntitiesWith() { return registry.view<T>(); }

        void serialise(bool binary = false);
        void deserialise(bool binary = false);

        template<typename Archive>
        void save(Archive& archive) const {
            //archive(cereal::make_nvp("Version", sceneVersion));
            archive(cereal::make_nvp("Scene Name", name));
        }

        template<typename Archive>
        void load(Archive& archive) {
            //archive(cereal::make_nvp("Version", sceneSerialisationVersion));
            archive(cereal::make_nvp("Scene Name", name));
        }

    private:
        template<typename T>
        void clone(const entt::registry& src) {
            auto view = src.view<const T>();
            for (const auto& [entity, component] : view.each()) {
                registry.emplace_or_replace<T>(entity, component);
            }
        }

        template<typename T>
        void clone(entt::entity dst, entt::entity src) {
            if (auto component = registry.try_get<T>(src))
                registry.emplace_or_replace<T>(dst, *component);
        }

        std::string name;
        SystemHolder systems;
        entt::registry registry;
        std::shared_ptr<Camera> camera;
        bool created{ false };
        bool runtime{ false };
    };
}
