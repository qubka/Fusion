#pragma once

#include "scene_camera.hpp"

#include <entt/entt.hpp>

namespace fe {
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
    struct DirtyTransformComponent {};

    /*struct BoundsComponent {
        glm::vec3 min{ -1.0f };
        glm::vec3 max{ 1.0f };
    };*/

    struct CameraComponent {
        SceneCamera camera;
        bool primary{ true };
        bool fixedAspectRatio{ false };
    };

    struct MeshComponent {
        std::string path;
        glm::vec3 scale{ 1.0f };
        glm::vec3 center{ 0.0f };
        glm::vec2 uvscale{ 1.0f };

        // Storage for runtime
        void* runtimeModel{ nullptr };
    };
    struct DirtyMeshComponent {};

    struct RigidbodyComponent {
        enum class BodyType { Static = 0, Dynamic = 1 };
        BodyType type{ BodyType::Static };
        //Layer layer;
        float mass{ 1.0f };
        float linearDrag{ 0.0f };
        float angularDrag{ 0.05f };
        bool disableGravity{ false };
        bool kinematic{ false };
        glm::bvec3 freezePosition{ false };
        glm::bvec3 freezeRotation{ false };

        // Storage for runtime
        void* runtimeBody{ nullptr };
    };

    struct BoxColliderComponent {
        glm::vec3 extent{ 1.0f };
        bool trigger{ false };

        // Storage for runtime
        void* runtimeShape{ nullptr };
    };

    struct SphereColliderComponent {
        float radius{ 0.0f };
        bool trigger{ false };

        // Storage for runtime
        void* runtimeShape{ nullptr };
    };

    struct CapsuleColliderComponent {
        float radius{ 0.0f };
        float height{ 0.0f };
        bool trigger{ false };

        // Storage for runtime
        void* runtimeShape{ nullptr };
    };

    struct PlaneColliderComponent {
        bool trigger{ false };
        // Storage for runtime
        void* runtimeShape{ nullptr };
    };

    /*struct MeshColliderComponent {
        // Storage for runtime
        void* runtimeShape{ nullptr };
    };*/

    struct PhysicsMaterialComponent {
        enum class CombineMode { Average = 0, Minimum, Multiply, Maximum };

        float dynamicFriction{ 0.5f };
        float staticFriction{ 0.5f };
        float restitution{ 0.5f };
        CombineMode frictionCombine{ CombineMode::Average };
        CombineMode restitutionCombine{ CombineMode::Average };

        // Storage for runtime
        void* runtimeMaterial{ nullptr };
    };
}
