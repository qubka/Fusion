#pragma once

namespace fe {
    struct SphereColliderComponent {
        glm::vec3 center{ 0.0f };
        float radius{ 0.0f };
        bool trigger{ false };
        //bool showColliderBounds{ true };

        // Storage for runtime
        void* runtime{ nullptr };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("center", center));
            archive(cereal::make_nvp("radius", radius));
            archive(cereal::make_nvp("trigger", trigger));
        }
    };
}