#pragma once

#include "components.hpp"

namespace entt {
    /** Improved version of registry with child-parent relationship. **/
    class world : public basic_registry<entity> {
    public:
        /**
         * Check if an entity is a parent.
         * @param entity A valid identifier.
         * @return True if entity is a parent.
         */
        [[nodiscard]] bool has_children(const entity_type entity) const;

        /**
         * Get the parent of an entity if exists.
         * @param entity A valid identifier.
         * @return Parent or nullptr if not exist.
         */
        [[nodiscard]] entity_type get_parent(const entity_type entity) const;

        /**
         * Removes a parent and all its children.
         * @param entity A valid identifier.
         */
        void remove_parent(const entity_type entity);

        /**
         * Destroy a parent entity and all its children.
         * @param entity A valid identifier.
         */
        void destroy_parent(const entity_type entity);

        /**
         * Assign an entity to a parent as a child.
         * @param parent Parent entity of a relationship.
         * @param child Child entity to form a relationship with a parent.
         */
        void assign_child(const entity_type parent, const entity_type child);

        /**
         * Remove a child entity from a parent entity.
         * @param parent Parent entity of a relationship.
         * @param child Child entity to remove.
         */
        void remove_child(const entity_type parent, const entity_type child);

        /**
         * Get all children of a parent entity.
         * @param entity A valid identifier.
         * @return Vector of childen, or nullptr if not parent. Will be empty if no children, not nullptr.
         */
        [[nodiscard]] std::vector<entity> get_children(const entity_type entity) const;

        void notify_children(const entity entity);

        [[nodiscard]] glm::mat4 transform(const entity_type entity) const;
        [[nodiscard]] glm::mat4 make_local_to_world(const entity_type entity) const;
        [[nodiscard]] glm::mat4 make_world_to_local(const entity_type entity) const;
    };
}
