#pragma once

#include "fusion/scene/scene_camera.hpp"

#include <cereal/cereal.hpp>

namespace fe {
    struct CameraComponent {
        SceneCamera camera;
        bool primary{ true };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Camera", camera),
                    cereal::make_nvp("Primary", primary));
        }
    };
}