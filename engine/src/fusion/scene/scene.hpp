#pragma once

#include "fusion/scene/system_holder.hpp"

#include <entt/entity/registry.hpp>

namespace fe {
    class Camera;
    class Scene {
        friend class Scenes;
    public:
        /**
         * Creates a new scene.
         * @param camera The scenes camera.
         */
        explicit Scene(std::shared_ptr<Camera> camera);
        ~Scene() = default;

        /**
         * Run when switching to this scene from another.
         */
        virtual void onStart();

        /**
         * Run when updating the scene.
         */
        virtual void onUpdate();

        /**
         * Run when the scene is start the runtime.
         */
        void onRuntimeStart();

        /**
         * Run when the scene is stop the runtime.
         */
        void onRuntimeStop();

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
         * @param camera The new camera.
         */
        void setCamera(std::shared_ptr<Camera> camera) { this->camera = std::move(camera); }

        /**
         * Gets if the scene is in runtime.
         * @return If the scene is in runtime.
         */
        bool isRuntime() { return runtime; }

        entt::entity createEntity(std::string name = "");

        void destroyEntity(entt::entity entity);

        entt::entity duplicateEntity(entt::entity entity);

        //void serialise(std::filesystem::path filename, bool binary = false);
        //void deserialise(std::filesystem::path filename, bool binary = false);

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
        void setRegistry(entt::registry&& registry) { this->registry = std::move(registry) ; }

        template<typename T>
        auto getAllEntitiesWith() { return registry.view<T>(); }

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

        //std::string name;
        SystemHolder systems;
        entt::registry registry;
        std::shared_ptr<Camera> camera;
        bool started{ false };
        bool runtime{ false };
    };
}
