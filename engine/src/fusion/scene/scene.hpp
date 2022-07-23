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

        /**
         * Creates an entity with default components.
         * @param name Name string.
         * @return The Entity handle that you can use to access the entity.
         */
        entt::entity createEntity(std::string name = "");

        /**
         * Destroys an entity.
         * @param entity The Entity handle of the entity to destroy.
         */
        void destroyEntity(entt::entity entity);

        /**
         * Clones an entity.
         * @param entity The Entity handle of the entity to clone.
         * @param parent Optional parent of created entity.
         * @return The Entity handle of the entity to destroy.
         */
        entt::entity duplicateEntity(entt::entity entity, entt::entity parent = entt::null);

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
        //void setRegistry(entt::registry&& reg) { registry = std::move(reg) ; }

        template<typename T>
        auto getAllEntitiesWith() { return registry.view<T>(); }

        void serialise(fs::path filepath = "", bool binary = false);
        void deserialise(fs::path filepath = "", bool binary = false);

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

    protected:
        /**
         * Called when the scene is created.
         */
        virtual void onStart();

        /**
         * Run when updating the scene.
         */
        virtual void onUpdate();

        /**
         * Called when scene is being activated, and will begin being rendered/updated.
         */
        virtual void onPlay();

        /**
         * Called when scene is being swapped and will no longer be rendered/updated
         */
        virtual void onStop();

    private:
        template<typename T>
        void copyComponent(entt::entity dst, entt::entity src) {
            if (auto component = registry.try_get<T>(src))
                registry.emplace_or_replace<T>(dst, *component);
        }

        template <typename... Component>
        void copyEntity(entt::entity dst, entt::entity src) {
            (copyComponent<Component>(dst, src), ...);
        }

        std::string name;
        entt::registry registry;
        SystemHolder systems;
        bool started{ false };
        bool runtime{ false };
    };
}
