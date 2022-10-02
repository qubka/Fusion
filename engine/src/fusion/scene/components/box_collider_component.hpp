#pragma once

namespace fe {
    struct BoxColliderComponent {
        glm::vec3 center{ 0.0f };
        glm::vec3 extent{ 1.0f };
        bool trigger{ false };
        //bool showColliderBounds{ true };

        // Storage for runtime
        void* runtime{ nullptr };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("center", center));
            archive(cereal::make_nvp("extent", extent));
            archive(cereal::make_nvp("trigger", trigger));
        }
    };
}