#pragma once

#include <entt/entity/registry.hpp>
#include <entt/entity/entity.hpp>
#include <cereal/cereal.hpp>

namespace fe {
    struct HierarchyComponent {
        entt::entity parent{ entt::null }; // the entity identifier of the parent, if any.
        entt::entity first{ entt::null }; //! the entity identifier of the first child, if any.
        entt::entity prev{ entt::null }; // the previous sibling in the list of children for the parent.
        entt::entity next{ entt::null }; // the next sibling in the list of children for the parent.
        //size_t children{ 0 }; //! the number of children for the given entity.

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Parent", parent),
                    cereal::make_nvp("First", first),
                    cereal::make_nvp("Prev", prev),
                    cereal::make_nvp("Next", next)
                    ); //cereal::make_nvp("Children", children)
        }
    };

    // TODO:: Move in system class
    class HierarchySystem {
    public:
        static void Init(entt::registry& registry, bool enable = true);

        // update hierarchy components when hierarchy component is modify
        static void OnConstruct(entt::registry& registry, entt::entity entity);
        static void OnDestroy(entt::registry& registry, entt::entity entity);
        static void OnUpdate(entt::registry& registry, entt::entity entity);

        static void Reparent(entt::entity entity, entt::entity parent, entt::registry& registry, HierarchyComponent& hierarchy);

        static bool IsParent(entt::entity entity, entt::entity child, entt::registry& registry);

        static void DestroyParent(entt::entity entity, entt::registry& registry);

        static void SetParent(entt::entity entity, entt::entity parent, entt::registry& registry);
    };
}
