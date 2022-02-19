#pragma once

#include "scene_camera.hpp"

namespace Fusion {

    struct  TagComponent {
        std::string tag;

        std::string& operator*() { return tag; }
        const std::string& operator*() const { return tag; }
        operator std::string() const { return tag; }
    };

    struct  TransformComponent {
        glm::vec3 translation{0};
        glm::vec3 rotation{0};
        glm::vec3 scale{1};

        operator glm::mat4() const {
            glm::mat4 m{1};
            return glm::translate(m, translation)
                   * glm::mat4_cast(glm::quat(rotation))
                   * glm::scale(m, scale);
        };

        /*operator glm::mat3() const {
            return glm::transpose(glm::inverse(glm::mat3(push.model)))
        }*/
    };

    class Mesh;
    struct  MeshRendererComponent {
        std::shared_ptr<Mesh> mesh;
    };

    /*struct  TranslateComponent {
        glm::mat4 transform{1};

        glm::mat4& operator*() { return transform; }
        const glm::mat4& operator*() const { return transform; }
        operator glm::mat4() const { return transform; }
    };

    struct  PositionComponent {
        glm::vec3 position{0};

        glm::vec3& operator*() { return position; }
        const glm::vec3& operator*() const { return position; }
        operator glm::vec3() const { return position; }
    };

    struct  RotationComponent {
        glm::quat rotation{1, 0, 0, 0};

        glm::quat& operator*() { return rotation; }
        const glm::quat& operator*() const { return rotation; }
        operator glm::quat() const { return rotation; }
    };

    struct  ScaleComponent {
        glm::vec3 scale{1};

        glm::vec3& operator*() { return scale; }
        const glm::vec3& operator*() const { return scale; }
        operator glm::vec3() const { return scale; }
    };*/

    struct  CameraComponent {
        SceneCamera camera;
        bool primary{true};
        bool fixedAspectRatio{false};
    };
}
