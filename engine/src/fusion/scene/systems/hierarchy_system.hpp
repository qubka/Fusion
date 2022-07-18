#pragma once

#include "fusion/scene/system.hpp"
#include "fusion/scene/components.hpp"

namespace fe {
    class HierarchySystem final : public System {
    public:
        explicit HierarchySystem(entt::registry& registry);
        ~HierarchySystem() override;

        /**
         * @brief Get the parent of an entity if exists.
         * @param entity A valid identifier.
         * @return Parent or null if not exist.
         */
        entt::entity getParent(entt::entity entity) const;

        /**
         * @brief Checking that the child has a certain parent.
         * @param parent Parent entity of a relationship.
         * @param child Child entity to form a relationship with a parent.
         * @return True if found, false otherwise.
         */
        bool isParent(entt::entity parent, entt::entity child) const;

        /**
         * @brief Removes a parent and all its children.
         * @param entity A valid identifier.
         */
        void removeParent(entt::entity entity);

        /**
         * @brief Destroy a parent entity and all its children.
         * @param entity A valid identifier.
         */
        void destroyParent(entt::entity entity);

        /**
         * @brief Assign an entity to a parent as a child.
         * @param parent Parent entity of a relationship.
         * @param child Child entity to form a relationship with a parent.
         */
        void assignChild(entt::entity parent, entt::entity child);

        /**
         * @brief Remove a child entity from a parent entity.
         * @param parent Parent entity of a relationship.
         * @param child Child entity to remove.
         */
        void removeChild(entt::entity parent, entt::entity child);

        /**
         * @brief Call function to all children of a parent entity.
         * @param entity A valid identifier.
         */
        std::vector<entt::entity> getChildren(entt::entity entity) const;

        /**
         * @brief Check if an entity is a parent.
         * @param entity A valid identifier.
         * @return True if entity is a parent.
         */
        bool hasChildren(entt::entity entity) const;

    private:
        void update(entt::entity entity);

        void onPlay() override;
        void onUpdate() override;
        void onStop() override;
        void onEnabled() override;
        void onDisabled() override;

        //static void OnConstruct(entt::registry& registry, entt::entity entity);
        //static void OnDestroy(entt::registry& registry, entt::entity entity);
        //static void OnUpdate(entt::registry& registry, entt::entity entity);
    };
}
