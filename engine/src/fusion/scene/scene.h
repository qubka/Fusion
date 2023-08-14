#pragma once

#include "fusion/scene/system_holder.h"

namespace fe {
    class Camera;
    class FUSION_API Scene {
        friend class SceneManager;
    public:
        /**
         * Creates a new scene.
         * @param name The scenes name.
         */
        explicit Scene(std::string_view name);
        explicit Scene(const Scene& other);
        ~Scene() = default;

        /**
         * Checks whether a system exists or not.
         * @tparam T The system type.
         * @return If the system exists.
         */
        template<typename T>
        bool hasSystem() const {
            return systems.has<T>();
        }

        /**
         * Gets a system.
         * @tparam T The system type.
         * @return The system.
         */
        template<typename T>
        T* getSystem() const {
            return systems.get<T>();
        }

        /**
         * Adds a system.
         * @tparam T The system type.
         * @tparam Args The constructor arg types.
         * @param args The constructor args.
         * @return The system.
         */
        template<typename T, typename... Args>
        void addSystem(Args&&...args) {
            systems.add<T>(std::make_unique<T>(registry, std::forward<Args>(args)...));
        }

        /**
         * Removes a system.
         * @tparam T The system type.
         */
        template<typename T>
        void removeSystem() {
            systems.remove<T>();
        }

        /**
         * Removes all systems.
         */
        void clearSystems();

        /**
         * Removes all entities.
         */
        void clearEntities();

        /**
         * Gets the primary camera component.
         * @return The camera component.
         */
        const Camera* getCamera() const;

        /**
         * Gets the primary camera entity.
         * @return The camera entity.
         */
        entt::entity getCameraEntity() const;

        /**
         * Gets the name of the scene.
         * @return The scene name.
         */
        const std::string& getName() const { return name; }

        /**
         * Sets the name to the scene.
         * @param str The scene name.
         */
        void setName(std::string_view str) { name = str; }

        /**
         * Gets if the scene is in runtime.
         * @return If the scene is in runtime.
         */
        bool isRuntime() const { return started && runtime; }

        /**
         * Creates an entity with default components.
         * @param name Name string.
         * @return The Entity handle that you can use to access the entity.
         */
        entt::entity createEntity(std::string name = "");

        /**
         * Destroys an entity.
         * @param entity The Entity handle of the entity to destroy.
         * @return True on success, false otherwise.
         */
        bool destroyEntity(entt::entity entity);

        /**
         * Clones an entity.
         * @param entity The Entity handle of the entity to clone.
         * @param parent Optional parent of created entity.
         * @return The Entity handle of the entity to destroy.
         */
        entt::entity duplicateEntity(entt::entity entity, entt::entity parent = entt::null);

        entt::entity getEntityByName(std::string_view name);
        bool isEntityValid(entt::entity entity);

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
        //void setRegistry(entt::registry&& registry) { this->registry = std::move(registry) ; }

        template<typename T>
        auto getAllEntitiesWith() { return registry.view<T>(); }

        void serialise(fs::path filepath = "", bool binary = false);
        void deserialise(fs::path filepath = "", bool binary = false);

        template<typename Archive>
        void save(Archive& archive) const {
            //archive(cereal::make_nvp("version", sceneVersion));
            archive(cereal::make_nvp("name", name));
        }

        template<typename Archive>
        void load(Archive& archive) {
            //archive(cereal::make_nvp("version", sceneSerialisationVersion));
            archive(cereal::make_nvp("name", name));
        }

        void importMesh(fs::path filepath);

    protected:
        /**
         * Called when the scene is created.
         */
        void onStart();

        /**
         * Run when updating the scene.
         */
        void onUpdate();

        /**
         * Called when scene is being activated, and will begin being rendered/updated.
         */
        void onPlay(); //

        /**
         * Called when scene is being swapped and will no longer be rendered/updated
         */
        void onStop(); //

    private:
        template<typename... T>
        void copyRegistry(const entt::registry& src) {
            (copyComponents<T>(src), ...);
        }

        template<typename T>
        void copyComponents(const entt::registry& src) {
            auto view = src.view<T>();
            for (const auto& [entity, component] : view.each())
                registry.emplace_or_replace<T>(entity, component);
        }

        template<typename T>
        void copyComponent(entt::entity dst, entt::entity src) {
            if (auto component = registry.try_get<T>(src))
                registry.emplace_or_replace<T>(dst, *component);
        }

        template<typename... T>
        void copyEntity(entt::entity dst, entt::entity src) {
            (copyComponent<T>(dst, src), ...);
        }

        std::string name;
        entt::registry registry;
        SystemHolder systems;
        bool runtime{ false };
        bool started{ false };
    };
}
