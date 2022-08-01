#pragma once

namespace fe {
    struct LightComponent {
        enum class LightType : unsigned char { Point = 0, Spot = 1, Directional = 2 };
        LightType type{ LightType::Point };

        glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
        float radius{ 0.0f };

        float cutOff{ 0.0f };
        float outerCutOff{ 0.0f };

        glm::vec3 ambient{ 0.1f };
        glm::vec3 diffuse{ 0.8f };
        glm::vec3 specular{ 1.8f };
        float constant{ 1.0f };
        float linear{ 0.09f };
        float quadratic{ 0.032f };

        template<typename Archive>
        void serialize(Archive& archive) {
        }
    };
}
