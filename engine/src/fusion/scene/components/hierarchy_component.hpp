#pragma once

namespace fe {
    struct HierarchyComponent {
        uint32_t children{ 0 }; //! the number of children for the given entity.
        entt::entity parent{ entt::null }; // the entity identifier of the parent, if any.
        entt::entity first{ entt::null }; //! the entity identifier of the first child, if any.
        entt::entity prev{ entt::null }; // the previous sibling in the list of children for the parent.
        entt::entity next{ entt::null }; // the next sibling in the list of children for the parent.

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("children", children),
                    cereal::make_nvp("parent", parent),
                    cereal::make_nvp("first", first),
                    cereal::make_nvp("prev", prev),
                    cereal::make_nvp("next", next));
        }
    };
}
