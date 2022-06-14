#pragma once

#include "system.hpp"

namespace fe {
    class DisabledSystem final : public System {
    public:
        DisabledSystem(entt::registry& registry) : System{registry} {}

        template<typename Component>
        auto status(const entt::entity entity) const {
            assert(has_component<Component>(registry, entity) != has_component<Component>(disabled, entity));
            return has_component<Component>(registry, entity);
        }

        template<typename Component>
        void disable(const entt::entity entity) {
            move_between<Component>(entity, registry, /* --> */ disabled);
        }

        template<typename Component>
        void enable(const entt::entity entity) {
            move_between<Component>(entity, disabled, /* --> */ registry);
        }

        template<typename Component>
        void toggle(const entt::entity entity) {
            if (status<Component>(entity)) {
                enable<Component>(entity);
            } else {
                disable<Component>(entity);
            }
        }

    private:
        template<typename Component>
        void has_component(const entt::registry& reg, entt::entity entity) {
            return reg.valid(entity) ? reg.all_of<Component>(entity) : false;
        }

        template<typename Component>
        void move_between(const entt::entity src, entt::registry& from, entt::registry& to) {
            auto const dst = to.valid(src) ? src : to.create(src);
            assert(dst == src);
            to.emplace_or_replace<Component>(dst, from.get<Component>(src));
            from.erase<Component>(src);
        }

        entt::registry disabled;
    };
}
