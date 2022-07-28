#pragma once

#include "fusion/graphics/cameras/camera.hpp"

namespace fe {
    struct CameraComponent : Camera {
        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Fov Degrees", fovDegrees),
                    cereal::make_nvp("Near Clip", nearClip),
                    cereal::make_nvp("Far Clip", farClip),
                    cereal::make_nvp("Aspect Ratio", aspectRatio),
                    cereal::make_nvp("Pivot Distance", pivotDistance),
                    cereal::make_nvp("Scale", scale),
                    cereal::make_nvp("Aperture", aperture),
                    cereal::make_nvp("Shutter Speed", shutterSpeed),
                    cereal::make_nvp("Sensitivity", sensitivity),
                    cereal::make_nvp("Shadow Bounding Radius", shadowBoundingRadius),
                    cereal::make_nvp("Orthographic", orthographic));
        }
    };
}