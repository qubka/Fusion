#pragma once

namespace fe {
    class Camera {
    public:
        const glm::mat4& getProjection() const { return projectionMatrix; }
        void setProjection(const glm::mat4& matrix) { projectionMatrix = matrix; }

        const glm::mat4& getView() const { return viewMatrix; }
        void setView(const glm::mat4& matrix) { viewMatrix = matrix; }

        const glm::mat4& getViewProjection() const { return viewProjectionMatrix; }
        void setViewProjection(const glm::mat4& matrix) { viewProjectionMatrix = matrix; }

        const glm::mat4& getTransform() const { return transformMatrix; };

    protected:
        glm::mat4 projectionMatrix{ 1.0f };
        glm::mat4 viewMatrix{ 1.0f };
        glm::mat4 viewProjectionMatrix{ 1.0f };

        glm::mat4 transformMatrix{ 1.0f };

        bool dirty{ true };
    };
}
