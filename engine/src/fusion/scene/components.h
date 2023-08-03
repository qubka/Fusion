#pragma once

#include <cereal/cereal.hpp>

#include "components/name_component.h"
#include "components/active_component.h"
#include "components/hierarchy_component.h"
#include "components/transform_component.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "components/material_component.h"
#include "components/script_component.h"

#include "components/rigidbody_component.h"
#include "components/box_collider_component.h"
#include "components/capsule_collider_component.h"
#include "components/mesh_collider_component.h"
#include "components/sphere_collider_component.h"
#include "components/physics_material_component.h"

#if FUSION_SCRIPRING
#define ALL_COMPONENTS NameComponent, ActiveComponent, HierarchyComponent, TransformComponent, CameraComponent, MeshComponent, LightComponent, MaterialComponent, ScriptComponent, RigidbodyComponent, BoxColliderComponent, CapsuleColliderComponent, MeshColliderComponent, SphereColliderComponent, PhysicsMaterialComponent
#else
#define ALL_COMPONENTS NameComponent, ActiveComponent, HierarchyComponent, TransformComponent, CameraComponent, MeshComponent, LightComponent, MaterialComponent, RigidbodyComponent, BoxColliderComponent, CapsuleColliderComponent, MeshColliderComponent, SphereColliderComponent, PhysicsMaterialComponent
#endif
namespace fe {
    template<typename... Component>
    struct ComponentGroup {
    };
    using AllComponents = ComponentGroup<ALL_COMPONENTS>;
}