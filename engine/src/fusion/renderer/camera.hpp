#pragma once

namespace Fusion {
    class Camera {
    public:
        Camera() = default;
        virtual ~Camera() = default;

        glm::mat4& getProjection() { return projectionMatrix; }
        glm::mat4& getView() { return viewMatrix; }
        glm::mat4& getViewProjection() { return viewProjectionMatrix; }

    protected:
        glm::mat4 projectionMatrix{1.0f};
        glm::mat4 viewMatrix{1.0f};
        glm::mat4 viewProjectionMatrix{1.0f};

        bool isDirty{true};
    };
}
