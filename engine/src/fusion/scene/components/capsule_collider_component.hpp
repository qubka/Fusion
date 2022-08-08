#pragma once

namespace fe {
    struct CapsuleColliderComponent {
        glm::vec3 center{ 0.0f };
        float radius{ 0.0f };
        float height{ 0.0f };
        bool trigger{ false };
        //bool showColliderBounds{ true };

        // Storage for runtime
        void* runtime{ nullptr };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("center", center),
                    cereal::make_nvp("radius", radius),
                    cereal::make_nvp("height", height),
                    cereal::make_nvp("trigger", trigger));
        }
    };
}