#pragma once

#include "fusion/geometry/transform.hpp"

namespace fe {
    struct TransformComponent : public Transform {
        TransformComponent() = default;
        TransformComponent(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
            : Transform{position, rotation, scale} {
        }

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("position", localPosition),
                    cereal::make_nvp("orientation", localOrientation),
                    cereal::make_nvp("scale", localScale));
        }
    };
}