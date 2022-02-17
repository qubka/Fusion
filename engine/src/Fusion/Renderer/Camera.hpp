#pragma once

namespace Fusion {
    class FUSION_API Camera {
    public:
        Camera() = default;
        virtual ~Camera() = default;

        glm::mat4& getProjection() { return projectionMatrix; };
        glm::mat4& getView() { return viewMatrix; };
        glm::mat4& getViewProjection() { return viewProjectionMatrix; };

    protected:
        glm::mat4 projectionMatrix{1};
        glm::mat4 viewMatrix{1};
        glm::mat4 viewProjectionMatrix{1};

        bool isDirty{true};
    };
}
