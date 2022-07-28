#pragma once

#include <entt/entity/registry.hpp>
#include <entt/entity/entity.hpp>

namespace fe {
    struct HierarchyComponent {
        uint32_t children{ 0 }; //! the number of children for the given entity.
        entt::entity parent{ entt::null }; // the entity identifier of the parent, if any.
        entt::entity first{ entt::null }; //! the entity identifier of the first child, if any.
        entt::entity prev{ entt::null }; // the previous sibling in the list of children for the parent.
        entt::entity next{ entt::null }; // the next sibling in the list of children for the parent.

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Children", children),
                    cereal::make_nvp("Parent", parent),
                    cereal::make_nvp("First", first),
                    cereal::make_nvp("Prev", prev),
                    cereal::make_nvp("Next", next));
        }
    };
}
