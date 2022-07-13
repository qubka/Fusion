#pragma once

#include "fusion/utils/cereal_extention.hpp"

#include <cereal/cereal.hpp>

namespace fe {
    class SceneCamera {
    public:
        enum class ProjectionType { Perspective = 0, Orthographic = 1 };
    public:
        SceneCamera();

        void setPerspective(float fov, float nearClip, float farClip);
        void setOrthographic(float size, float nearClip, float farClip);

        const glm::mat4& getProjection() const { return projectionMatrix; }

        //void setViewportSize(uint32_t width, uint32_t height);

        float getFOV() const { return fov; } // used as size for orthographic
        void setFOV(float verticalFov) { fov = verticalFov; recalculateProjection(); }
        float getNearClip() const { return nearClip; }
        void setNearClip(float near) { nearClip = near; recalculateProjection(); }
        float getFarClip() const { return farClip; }
        void setFarClip(float far) { farClip = far; recalculateProjection(); }

        ProjectionType getProjectionType() const { return projectionType; }
        void setProjectionType(ProjectionType type) { projectionType = type; recalculateProjection(); }

        bool isPerspective() const { return projectionType == ProjectionType::Perspective; }

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Projection Type", projectionType),
                    cereal::make_nvp("Fov", fov),
                    cereal::make_nvp("Near Clip", nearClip),
                    cereal::make_nvp("Far CLip", farClip),
                    cereal::make_nvp("Aspect Ratio", aspectRatio));
        }

    private:
        void recalculateProjection();

    private:
        ProjectionType projectionType{ ProjectionType::Perspective };
        glm::mat4 projectionMatrix{ 1.0f };

        float fov{ 35.0f }; // vertical field of view in degrees
        float aspectRatio{ 1.0f };
        float nearClip{ 0.1f };
        float farClip{ 1000.f };
    };
}
