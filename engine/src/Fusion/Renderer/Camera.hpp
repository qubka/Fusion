#pragma once

namespace Fusion {
    class FUSION_API Camera {
    public:
        Camera() = default;
        virtual ~Camera() = default;

        const glm::mat4& getProjection() const { return projectionMatrix; };
        const glm::mat4& getView() const { return viewMatrix; };
        const glm::mat4& getViewProjection() const { return viewProjectionMatrix; };

    protected:
        glm::mat4 projectionMatrix{1};
        glm::mat4 viewMatrix{1};
        glm::mat4 viewProjectionMatrix{1};

        bool isDirty{true};
    };
}
