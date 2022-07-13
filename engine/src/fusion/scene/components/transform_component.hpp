#pragma once

#include <cereal/cereal.hpp>

namespace fe {
    struct TransformComponent {
        glm::vec3 position{ 0.0f };
        glm::quat rotation{ quat::identity };
        glm::vec3 scale{ 1.0f };

        glm::mat4 model{ 1.0f };

        glm::mat4 getTransform() const {
            return glm::mat4{ //translate
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 0.0f, 1.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 1.0f, 0.0f },
                    { position, 1.0f }
            }
                   * glm::mat4_cast(rotation) //rotate
                   * glm::mat4{ //scale
                    { scale.x, 0.0f, 0.0f, 0.0f },
                    { 0.0f, scale.y, 0.0f, 0.0f },
                    { 0.0f, 0.0f, scale.z, 0.0f },
                    { 0.0f, 0.0f, 0.0f, 1.0f }
            };
        }

        glm::vec3 getUp() const { return rotation * vec3::up; }
        glm::vec3 getForward() const { return rotation * vec3::forward; }
        glm::vec3 getRight() const { return rotation * vec3::right; }
        glm::vec3 transformDirection(const glm::vec3& direction) const { return glm::rotate(rotation, direction); }
        glm::vec3 transformVector(const glm::vec3& vector) const { return glm::rotate(rotation, scale * vector); }
        glm::vec3 transformPoint(const glm::vec3& point) const { return glm::rotate(rotation, scale * point) + position; }
        glm::vec3 inverseTransformDirection(const glm::vec3& direction) const { return glm::rotate(glm::inverse(rotation), direction); }
        glm::vec3 inverseTransformVector(const glm::vec3& vector) const { return glm::rotate(glm::inverse(rotation), vector / scale); }
        glm::vec3 inverseTransformPoint(const glm::vec3& point) const { return glm::rotate(glm::inverse(rotation), (point - position) / scale); }

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Position", position),
                    cereal::make_nvp("Rotation", rotation),
                    cereal::make_nvp("Scale", scale));
        }
    };
    struct DirtyTransformComponent {};
}