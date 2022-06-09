#pragma once

#include "components.hpp"

namespace entt {
    /** Improved version of registry with child-parent relationship. **/
    class world : public entt::basic_registry<entity> {
    public:
        /**
         * @brief Check if an entity is a parent.
         * @param entity A valid identifier.
         * @return True if entity is a parent.
         */
        [[nodiscard]] bool has_children(const entity_type entity) const;

        /**
         * @brief Get the parent of an entity if exists.
         * @param entity A valid identifier.
         * @return Parent or nullptr if not exist.
         */
        [[nodiscard]] entity_type get_parent(const entity_type entity) const;

        /**
         * @brief Removes a parent and all its children.
         * @param entity A valid identifier.
         */
        void remove_parent(const entity_type entity);

        /**
         * @brief Destroy a parent entity and all its children.
         * @param entity A valid identifier.
         */
        void destroy_parent(const entity_type entity);

        /**
         * @brief Assign an entity to a parent as a child.
         * @param parent Parent entity of a relationship.
         * @param child Child entity to form a relationship with a parent.
         */
        void assign_child(const entity_type parent, const entity_type child);

        /**
         * @brief Remove a child entity from a parent entity.
         * @param parent Parent entity of a relationship.
         * @param child Child entity to remove.
         */
        void remove_child(const entity_type parent, const entity_type child);

        /**
         * @brief Get all children of a parent entity.
         * @param entity A valid identifier.
         * @return Vector of childen, or nullptr if not parent. Will be empty if no children, not nullptr.
         */
        [[nodiscard]] std::vector<entity> get_children(const entity_type entity) const;

        /**
        * @brief Patches the given component for an entity with its children.
        * @tparam Component Type of component to patch.
        * @tparam Func Types of the function objects to invoke.
        * @param entity A valid identifier.
        * @param func Valid function objects.
        * @return A reference to the patched component.
        */
        template<typename Component, typename... Func>
        void patch_children(const entity_type entity, Func &&...func) {
            patch<Component>(entity, std::forward<Func>(func)...);
            if (auto p = try_get<fe::RelationshipComponent>(entity); p && p->children > 0) {
                auto curr = p->first;
                while (curr != entt::null) {
                    patch_children<Component>(curr);
                    curr = get<fe::RelationshipComponent>(curr).next;
                }
            }
        }

        template<typename Component>
        void clone(const basic_registry<entity>& src) {
            auto view = src.view<const Component>();
            for (auto [entity, component] : view.each()) {
                emplace<Component>(entity, component);
            }
        }

        [[nodiscard]] glm::mat4 transform(const entity_type entity) const;
    };
}
