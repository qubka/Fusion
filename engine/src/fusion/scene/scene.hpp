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
        explicit Scene(std::unique_ptr<Camera>&& camera);
        Scene(const Scene& scene);
        ~Scene() = default;

        /**
         * Run when switching to this scene from another.
         */
        virtual void start();

        /**
         * Run when updating the scene.
         */
        virtual void update();

        void runtimeStart();
        void runtimeStop();


        void updateRuntime();
        void updateEditor();

       // void OnViewportResize(const glm::vec2& size);



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
            systems.add<T>(std::make_unique<T>(registry, std::forward<Args>(args)...));
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
        Camera* getCamera() const { return camera; }

        /**
         * Sets the current camera to a new camera.
         * @param camera The new camera.
         */
        void setCamera(Camera* camera) { this->camera = camera; }

        /**
         * Gets if the scene is for runtime.
         * @return If the scene is runtime.
         */
        bool isRuntimeScene() { return runtimeScene; }

        /**
         * Gets if the scene is active.
         * @return If the scene is active.
         */
        bool isActive() { return active; }

        entt::entity createEntity(std::string name = "");

        void destroyEntity(entt::entity entity);

        entt::entity duplicateEntity(entt::entity entity);

        /**
         * Gets the scene entity registry.
         * @return Entity registry.
         */
        const entt::registry& getRegistry() const { return registry; }
        entt::registry& getRegistry() { return registry; }

        template<typename T>
        auto getAllEntitiesWith() { return registry.view<T>(); }

    private:
        template<typename T>
        void clone(const entt::registry& src) {
            auto view = src.view<const T>();
            for (auto [entity, component] : view.each()) {
                registry.emplace_or_replace<T>(entity, component);
            }
        }

        template<typename T>
        void clone(entt::entity dst, entt::entity src) {
            if (auto component = registry.try_get<T>(src))
                registry.emplace_or_replace<T>(dst, *component);
        }

        SystemHolder systems;
        entt::registry registry;
        Camera* camera;
        bool started{ false };
        bool active{ false };
        bool runtimeScene{ false };
    };
}
