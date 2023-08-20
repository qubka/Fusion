#pragma once

#include "fusion/graphics/cameras/camera.h"

namespace fe {
    struct CameraComponent final : public Camera {
        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("fovDegrees", fovDegrees));
            archive(cereal::make_nvp("nearClip", nearClip));
            archive(cereal::make_nvp("farClip", farClip));
            archive(cereal::make_nvp("aspectRatio", aspectRatio));
            archive(cereal::make_nvp("pivotDistance", pivotDistance));
            archive(cereal::make_nvp("scale", scale));
            archive(cereal::make_nvp("aperture", aperture));
            archive(cereal::make_nvp("shutterSpeed", shutterSpeed));
            archive(cereal::make_nvp("sensitivity", sensitivity));
            archive(cereal::make_nvp("shadowBoundingRadius", shadowBoundingRadius));
            archive(cereal::make_nvp("orthographic", orthographic));
        }
    };
}