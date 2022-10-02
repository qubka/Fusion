#pragma once

#include "fusion/graphics/cameras/camera.hpp"

namespace fe {
    struct CameraComponent : public Camera {
        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("fov degrees", fovDegrees));
            archive(cereal::make_nvp("near clip", nearClip));
            archive(cereal::make_nvp("far clip", farClip));
            archive(cereal::make_nvp("aspect ratio", aspectRatio));
            archive(cereal::make_nvp("pivot distance", pivotDistance));
            archive(cereal::make_nvp("scale", scale));
            archive(cereal::make_nvp("aperture", aperture));
            archive(cereal::make_nvp("shutter speed", shutterSpeed));
            archive(cereal::make_nvp("sensitivity", sensitivity));
            archive(cereal::make_nvp("shadow bounding radius", shadowBoundingRadius));
            archive(cereal::make_nvp("orthographic", orthographic));
        }
    };
}