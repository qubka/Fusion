#pragma once

#include "scene_camera.hpp"
#include "fusion/renderer/vkx/model.hpp"

#include <entt/entt.hpp>

namespace fe {

    enum Space {
        World,
        Local
    };

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

    struct RelationshipComponent {
        size_t children{ 0 }; //! the number of children for the given entity.
        entt::entity first{ entt::null }; //! the entity identifier of the first child, if any.
        entt::entity prev{ entt::null }; // the previous sibling in the list of children for the parent.
        entt::entity next{ entt::null }; // the next sibling in the list of children for the parent.
        entt::entity parent{ entt::null }; // the entity identifier of the parent, if any.
    };

    struct TransformComponent { // http://graphics.cs.cmu.edu/courses/15-466-f17/notes/hierarchy.html
        glm::vec3 position{ 0.0f };
        glm::quat rotation{ quat::identity };
        glm::vec3 scale{ 1.0f };

        glm::mat4 localToWorldMatrix{ 1.0f };
        glm::mat4 worldToLocalMatrix{ 1.0f };

        glm::mat4 transform() const {
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
    };
    struct DirtyTransformComponent { };

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
        std::shared_ptr<vkx::model::Model> model;
    };
    struct DirtyModelComponent { };

    struct CameraComponent {
        SceneCamera camera;
        bool primary{ true };
        bool fixedAspectRatio{ false };
    };
}
