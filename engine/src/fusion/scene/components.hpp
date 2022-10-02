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
#include "components/material_component.hpp"

#include "components/rigidbody_component.hpp"
#include "components/box_collider_component.hpp"
#include "components/capsule_collider_component.hpp"
#include "components/mesh_collider_component.hpp"
#include "components/sphere_collider_component.hpp"
#include "components/physics_material_component.hpp"

#define ALL_COMPONENTS IdComponent, NameComponent, ActiveComponent, HierarchyComponent, TransformComponent, CameraComponent, MeshComponent, LightComponent, MaterialComponent, \
    RigidbodyComponent, BoxColliderComponent, CapsuleColliderComponent, MeshColliderComponent, SphereColliderComponent, PhysicsMaterialComponent