#pragma once

#include <entt/entity/registry.hpp>

namespace fe {
    class HierarchySystem final {
    public:
        HierarchySystem(entt::registry& registry) : registry{ registry} {}

        /**
         * @brief Check if an entity is a parent.
         * @param entity A valid identifier.
         * @return True if entity is a parent.
         */
        [[nodiscard]] bool hasChildren(entt::entity entity) const;

        /**
         * @brief Get the parent of an entity if exists.
         * @param entity A valid identifier.
         * @return Parent or nullptr if not exist.
         */
        [[nodiscard]] entt::entity getParent(entt::entity entity) const;

        /**
         * @brief Checks that a parent entity has a specific child.
         * @param parent Parent entity of a relationship.
         * @param child Child entity to form a relationship with a parent.
         * @return Parent or nullptr if not exist.
         */
        [[nodiscard]] bool isParent(entt::entity parent, entt::entity child) const;

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
         * @brief Generate array of all children of a parent entity.
         * @param entity A valid identifier.
         * @return Array of all children.
         */
        std::vector<entt::entity> getChildren(entt::entity entity) const;

    protected:
        entt::registry& registry;
    };
}
