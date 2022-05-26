#pragma once

namespace fe {
    class Camera {
    public:
        glm::mat4& getProjection() { return projectionMatrix; }
        void setProjection(const glm::mat4& matrix) { projectionMatrix = matrix; }

        glm::mat4& getView() { return viewMatrix; }
        void setView(const glm::mat4& matrix) { viewMatrix = matrix; }

        glm::mat4& getViewProjection() { return viewProjectionMatrix; }
        void setViewProjection(const glm::mat4& matrix) { viewProjectionMatrix = matrix; }

    protected:
        glm::mat4 projectionMatrix{ 1.0f };
        glm::mat4 viewMatrix{ 1.0f };
        glm::mat4 viewProjectionMatrix{ 1.0f };

        bool dirty{ true };
    };
}
