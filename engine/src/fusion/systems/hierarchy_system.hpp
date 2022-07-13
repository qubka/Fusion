#pragma once

#include <entt/entity/registry.hpp>

namespace fe {
    class ParentSystem final {
    public:
        ParentSystem(entt::registry& registry) : registry{registry} {}

        /**
         * @brief Check if an entity is a parent.
         * @param entity A valid identifier.
         * @return True if entity is a parent.
         */
        [[nodiscard]] bool has_children(const entt::entity entity) const;

        /**
         * @brief Get the parent of an entity if exists.
         * @param entity A valid identifier.
         * @return Parent or nullptr if not exist.
         */
        [[nodiscard]] entt::entity get_parent(const entt::entity entity) const;

        /**
         * @brief Removes a parent and all its children.
         * @param entity A valid identifier.
         */
        void remove_parent(const entt::entity entity);

        /**
         * @brief Destroy a parent entity and all its children.
         * @param entity A valid identifier.
         */
        void destroy_parent(const entt::entity entity);

        /**
         * @brief Assign an entity to a parent as a child.
         * @param parent Parent entity of a relationship.
         * @param child Child entity to form a relationship with a parent.
         */
        void assign_child(const entt::entity parent, const entt::entity child);

        /**
         * @brief Remove a child entity from a parent entity.
         * @param parent Parent entity of a relationship.
         * @param child Child entity to remove.
         */
        void remove_child(const entt::entity parent, const entt::entity child);

        /**
         * @brief Call function to all children of a parent entity.
         * @param entity A valid identifier.
         */
        void each_child(const entt::entity entity, const std::function<void(const entt::entity)>& function) const;

    protected:
        entt::registry& registry;
    };
}
