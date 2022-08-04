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

#include "components/rigidbody_component.hpp"
#include "components/box_collider_component.hpp"
#include "components/capsule_collider_component.hpp"
#include "components/mesh_collider_component.hpp"
#include "components/sphere_collider_component.hpp"
#include "components/physics_material_component.hpp"

#define ALL_COMPONENTS IdComponent, NameComponent, ActiveComponent, HierarchyComponent, TransformComponent, CameraComponent, MeshComponent, LightComponent, \
    RigidbodyComponent, BoxColliderComponent, CapsuleColliderComponent, MeshColliderComponent, SphereColliderComponent, PhysicsMaterialComponent

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













    struct MaterialComponent {
        glm::vec4 ambient{ 1.0f };
        glm::vec4 diffuse{ 1.0f };
        glm::vec4 emission{ 1.0f };
        glm::vec3 specular{ 1.0f };
        float shininess{ 0.5f };
    };*/
}
