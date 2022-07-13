#pragma once

#include "scene_camera.hpp"

#include "fusion/utils/cereal_extention.hpp"
#include "fusion/utils/glm_extention.hpp"

#include <cereal/cereal.hpp>
#include <entt/entt.hpp>

//#include <uuid.h>

#define ALL_COMPONENTS NameComponent, ActiveComponent, HierarchyComponent, TransformComponent, CameraComponent, ModelComponent

namespace fe {
    struct NameComponent {
        std::string name;

        std::string& operator*() { return name; }
        const std::string& operator*() const { return name; }
        operator std::string() const { return name; }

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Name", name));
        }
    };

    struct ActiveComponent {
        bool active{ true };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Active", active));
        }
    };

    struct HierarchyComponent {
        size_t children{ 0 }; //! the number of children for the given entity.
        entt::entity first{ entt::null }; //! the entity identifier of the first child, if any.
        entt::entity prev{ entt::null }; // the previous sibling in the list of children for the parent.
        entt::entity next{ entt::null }; // the next sibling in the list of children for the parent.
        entt::entity parent{ entt::null }; // the entity identifier of the parent, if any.

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Children", children),
                    cereal::make_nvp("First", first),
                    cereal::make_nvp("Prev", prev),
                    cereal::make_nvp("Next", next),
                    cereal::make_nvp("Parent", parent));
        }
    };

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

    struct CameraComponent {
        SceneCamera camera;
        bool primary{ true };
        bool fixedAspectRatio{ false };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Camera", camera),
                    cereal::make_nvp("Primary", primary),
                    cereal::make_nvp("FixedAspectRatio", fixedAspectRatio));
        }
    };

    struct ModelComponent {
        fs::path path;
        //bool castShadows{ false };

        // Storage for runtime
        void* runtimeModel{ nullptr };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Path", path));
        }
    };
    struct DirtyMeshComponent {};

    /*struct PointLightComponent {
        glm::vec3 color{ 1.0f };
        float intensity{ 1.0f };
        float radius{ 2.0f };
    };

    struct DirectionalLightComponent {
        glm::vec3 color{ 1.0f };
        float intensity{ 1.0f };
    };

    struct ScriptComponent {
        std::string name;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Name", name));
        }
    };

    struct RigidBodyComponent {
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
        //bool showColliderBounds{ true };

        // Storage for runtime
        void* runtimeShape{ nullptr };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Extent", extent),
                    cereal::make_nvp("Trigger", trigger));
        }
    };

    struct SphereColliderComponent {
        float radius{ 0.0f };
        bool trigger{ false };
        //bool showColliderBounds{ true };

        // Storage for runtime
        void* runtimeShape{ nullptr };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Radius", radius),
                    cereal::make_nvp("Trigger", trigger));
        }
    };

    struct CapsuleColliderComponent {
        float radius{ 0.0f };
        float height{ 0.0f };
        bool trigger{ false };
        //bool showColliderBounds{ true };

        // Storage for runtime
        void* runtimeShape{ nullptr };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Radius", radius),
                    cereal::make_nvp("Radius", radius),
                    cereal::make_nvp("Trigger", trigger));
        }
    };

    struct MeshColliderComponent {
        bool convex{ false };
        bool trigger{ false };
        bool overrideMesh{ false };
        //bool showColliderBounds{ false };

        // Storage for runtime
        void* runtimeShape{ nullptr };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Convex", convex),
                    cereal::make_nvp("Trigger", trigger),
                    cereal::make_nvp("Override Mesh", overrideMesh));
        }
    };

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

    struct MaterialComponent {
        glm::vec4 ambient{ 1.0f };
        glm::vec4 diffuse{ 1.0f };
        glm::vec4 emission{ 1.0f };
        glm::vec3 specular{ 1.0f };
        float shininess{ 0.5f };
    };*/
}
