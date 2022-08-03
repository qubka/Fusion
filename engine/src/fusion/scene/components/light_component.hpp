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
            archive(cereal::make_nvp("type", type),
                    cereal::make_nvp("color", color),
                    cereal::make_nvp("radius", radius),
                    cereal::make_nvp("cutOff", cutOff),
                    cereal::make_nvp("outerCutOff", outerCutOff),
                    cereal::make_nvp("ambient", ambient),
                    cereal::make_nvp("diffuse", diffuse),
                    cereal::make_nvp("specular", specular),
                    cereal::make_nvp("constant", constant),
                    cereal::make_nvp("linear", linear),
                    cereal::make_nvp("quadratic", quadratic));
        }
    };
}
