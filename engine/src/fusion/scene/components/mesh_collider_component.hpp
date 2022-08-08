#pragma once

namespace fe {
    struct MeshColliderComponent {
        bool convex{ false };
        bool trigger{ false };
        bool overrideMesh{ false };
        //CookingOptions cookingOptions;
        //bool showColliderBounds{ false };

        // Storage for runtime
        void* runtime{ nullptr };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("convex", convex),
                    cereal::make_nvp("trigger", trigger),
                    cereal::make_nvp("override mesh", overrideMesh));
        }
    };
}