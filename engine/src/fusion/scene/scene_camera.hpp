#pragma once

#include "fusion/renderer/camera.hpp"

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

        float getPerspectiveVerticalFOV() const { return perspectiveFOV; }
        void setPerspectiveVerticalFOV(float verticalFov) { perspectiveFOV = verticalFov; recalculateProjection(); }
        float getPerspectiveNearClip() const { return perspectiveNear; }
        void setPerspectiveNearClip(float nearClip) { perspectiveNear = nearClip; recalculateProjection(); }
        float getPerspectiveFarClip() const { return perspectiveFar; }
        void setPerspectiveFarClip(float farClip) { perspectiveFar = farClip; recalculateProjection(); }

        float getOrthographicSize() const { return orthographicSize; }
        void setOrthographicSize(float size) { orthographicSize = size; recalculateProjection(); }
        float getOrthographicNearClip() const { return orthographicNear; }
        void setOrthographicNearClip(float nearClip) { orthographicNear = nearClip; recalculateProjection(); }
        float getOrthographicFarClip() const { return orthographicFar; }
        void setOrthographicFarClip(float farClip) { orthographicFar = farClip; recalculateProjection(); }

        ProjectionType getProjectionType() const { return projectionType; }
        void setProjectionType(ProjectionType type) { projectionType = type; recalculateProjection(); }

    private:
        void recalculateProjection();

    private:
        ProjectionType projectionType{ ProjectionType::Orthographic };
        glm::mat4 projectionMatrix{ 1.0f };

        float perspectiveFOV{ glm::radians(45.0f) };
        float perspectiveNear{ 0.01f };
        float perspectiveFar{ 1000.0f };

        float orthographicSize{ 10.0f };
        float orthographicNear{ -1.0f };
        float orthographicFar{ 1.0f };

        float aspectRatio{ 0.0f };
    };
}
