#pragma once

#include "scene_camera.hpp"

#include <entt/entity/registry.hpp>

namespace fe {

    /*static std::random_device RandomDevice;
    static std::mt19937_64 Engine(RandomDevice());
    static std::uniform_int_distribution<uint64_t> UniformDistribution;

    struct IdComponent {
        uint64_t id;

        IdComponent() : id{UniformDistribution(Engine)} {}
        explicit IdComponent(uint64_t id) : id{id} {}

        uint64_t& operator*() { return id; }
        const uint64_t& operator*() const { return id; }
        operator uint64_t() const { return id; }
    };*/

    struct TagComponent {
        std::string tag;

        std::string& operator*() { return tag; }
        const std::string& operator*() const { return tag; }
        operator std::string() const { return tag; }
    };

    struct TransformComponent {
        glm::vec3 translation{ 0.0f };
        glm::vec3 rotation{ 0.0f };
        glm::vec3 scale{ 1.0f };

        glm::mat4 transform() const {
            glm::mat4 m{ 1.0f };
            return glm::translate(m, translation)
                   * glm::mat4_cast(glm::quat(rotation))
                   * glm::scale(m, scale);
        };

        glm::mat4 invTransform() const {
            glm::mat4 m{ 1.0f };
            return glm::translate(m, translation)
                   * glm::mat4_cast(glm::quat{rotation})
                   * glm::scale(m, {scale.x, -scale.y, scale.z});
        };

        glm::mat4 normal() const {
            return glm::transpose(glm::inverse(glm::mat3{transform()}));
        }
    };

    struct RelationshipComponent {
        size_t children{ 0 }; //! the number of children for the given entity.
        entt::entity first{ entt::null }; //! the entity identifier of the first child, if any.
        entt::entity prev{ entt::null }; // the previous sibling in the list of children for the parent.
        entt::entity next{ entt::null }; // the next sibling in the list of children for the parent.
        entt::entity parent{ entt::null }; // the entity identifier of the parent, if any.
    };

    struct BoundsComponent {
        glm::vec3 min{ 0.0f };
        glm::vec3 max{ 0.0f };
        glm::vec3 size{ 0.0f };
    };

    struct ModelComponent {
        std::string path;
        glm::vec3 scale{ 1.0f };
        glm::vec3 center{ 0.0f };
        glm::vec2 uvscale{ 1.0f };
    };

    struct CameraComponent {
        SceneCamera camera;
        bool primary{ true };
        bool fixedAspectRatio{ false };
    };
}
