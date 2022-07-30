#pragma once

#include <cereal/cereal.hpp>

#include "components/id_component.hpp"
#include "components/name_component.hpp"
#include "components/active_component.hpp"
#include "components/hierarchy_component.hpp"
#include "components/transform_component.hpp"
#include "components/camera_component.hpp"
#include "components/mesh_component.hpp"
#include "components/light_component.hpp"

#define ALL_COMPONENTS IdComponent, NameComponent, ActiveComponent, HierarchyComponent, TransformComponent, CameraComponent, MeshComponent, LightComponent

namespace fe {
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
            archive(cereal::make_nvp("name", name));
        }
    };

    struct RigidBodyComponent {
        enum class BodyType uint8_t { Static = 0, Dynamic = 1 };
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
            archive(cereal::make_nvp("extent", extent),
                    cereal::make_nvp("trigger", trigger));
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
            archive(cereal::make_nvp("radius", radius),
                    cereal::make_nvp("trigger", trigger));
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
            archive(cereal::make_nvp("radius", radius),
                    cereal::make_nvp("radius", radius),
                    cereal::make_nvp("trigger", trigger));
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
            archive(cereal::make_nvp("convex", convex),
                    cereal::make_nvp("trigger", trigger),
                    cereal::make_nvp("override mesh", overrideMesh));
        }
    };

    struct PhysicsMaterialComponent {
        enum class CombineMode uint8_t { Average = 0, Minimum, Multiply, Maximum };

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
