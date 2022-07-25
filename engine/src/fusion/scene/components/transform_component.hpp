#pragma once

#include "fusion/geometry/transform.hpp"

#include <cereal/cereal.hpp>

namespace fe {
    struct TransformComponent : public Transform {

        template<typename Archive>
        void save(Archive& archive) const {
            archive(cereal::make_nvp("Position", getLocalPosition()),
                    cereal::make_nvp("Orientation", getWorldOrientation()),
                    cereal::make_nvp("Scale", getLocalScale()));
        }

        template<typename Archive>
        void load(Archive& archive) {
            glm::vec3 position;
            glm::quat orientation;
            glm::vec3 scale;
            archive(cereal::make_nvp("Position", position),
                    cereal::make_nvp("Orientation", orientation),
                    cereal::make_nvp("Scale", scale));
            setLocalPosition(position);
            setLocalOrientation(orientation);
            setLocalScale(scale);
        }
    };
}