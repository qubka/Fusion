#pragma once

#include "fusion/graphics/cameras/camera.hpp"

namespace fe {
    struct CameraComponent : public Camera {
        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("fov degrees", fovDegrees),
                    cereal::make_nvp("near clip", nearClip),
                    cereal::make_nvp("far clip", farClip),
                    cereal::make_nvp("aspect ratio", aspectRatio),
                    cereal::make_nvp("pivot distance", pivotDistance),
                    cereal::make_nvp("scale", scale),
                    cereal::make_nvp("aperture", aperture),
                    cereal::make_nvp("shutter speed", shutterSpeed),
                    cereal::make_nvp("sensitivity", sensitivity),
                    cereal::make_nvp("shadow bounding radius", shadowBoundingRadius),
                    cereal::make_nvp("orthographic", orthographic));
        }
    };
}