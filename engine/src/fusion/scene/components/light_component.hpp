#pragma once

namespace fe {
    struct LightComponent {
        glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
        float radius{ 0.0f };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("color", color),
                    cereal::make_nvp("radius", radius));
        }
    };
}
