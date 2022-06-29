#pragma once

namespace fe {
    class Camera {
    public:
        virtual void start() = 0;
        virtual void update(float dt) = 0;

        /**
         * Gets the projection matrix used in the current scene render.
         * @return The projection matrix.
         */
        const glm::mat4& getProjection() const { return projectionMatrix; }
        void setProjection(const glm::mat4& matrix) { projectionMatrix = matrix; }

        /**
         * Gets the view matrix created by the current camera position and rotation.
         * @return The view matrix.
         */
        const glm::mat4& getView() const { return viewMatrix; }
        void setView(const glm::mat4& matrix) { viewMatrix = matrix; }

        /**
         * Gets the view projection matrix created by the current camera view and projection matrix.
         * @return The view projection matrix.
         */
        const glm::mat4& getViewProjection() const { return viewProjectionMatrix; }
        void setViewProjection(const glm::mat4& matrix) { viewProjectionMatrix = matrix; }

        /**
         * Gets the model matrix created by the current camera position and rotation.
         * @return The model matrix.
         */
        const glm::mat4& getTransform() const { return transformMatrix; };

    protected:
        glm::mat4 projectionMatrix{ 1.0f };
        glm::mat4 viewMatrix{ 1.0f };
        glm::mat4 viewProjectionMatrix{ 1.0f };

        glm::mat4 transformMatrix{ 1.0f };

        bool dirty{ true };
    };
}
