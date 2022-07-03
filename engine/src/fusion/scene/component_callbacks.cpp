#include "component_callbacks.hpp"
#include "components.hpp"

using namespace fe;

template<typename T>
void onComponentConstruct(entt::registry& registry, entt::entity entity) {
}

template<typename T>
void onComponentUpdate(entt::registry& registry, entt::entity entity) {
}

template<typename T>
void onComponentDestroy(entt::registry& registry, entt::entity entity) {
}

///////////////////////////////////////////////////////////////////////

/*template<>
void onComponentUpdate<RigidBodyComponent>(entt::registry& registry, entt::entity entity) {
    if (!active) return;
    auto& rigidbody = registry.get<RigidBodyComponent>(entity);
    if (rigidbody.type == RigidBodyComponent::BodyType::Dynamic) {
        if (auto rigid = reinterpret_cast<PxRigidDynamic*>(rigidbody.runtimeBody)) {
            rigid->setMass(rigidbody.mass);
            rigid->setLinearDamping(rigidbody.linearDrag);
            rigid->setAngularDamping(rigidbody.angularDrag);
            rigid->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, rigidbody.kinematic);
            rigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, rigidbody.freezePosition.x);
            rigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, rigidbody.freezePosition.y);
            rigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, rigidbody.freezePosition.z);
            rigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, rigidbody.freezePosition.x);
            rigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, rigidbody.freezePosition.y);
            rigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, rigidbody.freezePosition.z);
        }
    }
}

template<>
void onComponentDestroy<RigidBodyComponent>(entt::registry& registry, entt::entity entity) {
    if (!active) return;
    auto& rigidbody = registry.get<RigidBodyComponent>(entity);
    if (auto body = reinterpret_cast<PxRigidActor*>(rigidbody.runtimeBody)) {
        scene->removeActor(*body);
    }
}

template<>
void onComponentUpdate<BoxColliderComponent>(entt::registry& registry, entt::entity entity) {
    if (!active) return;
    auto& collider = registry.get<BoxColliderComponent>(entity);
    if (auto shape = reinterpret_cast<PxShape*>(collider.runtimeShape)) {
        auto& transform = registry.get<TransformComponent>(entity);
        glm::vec3 halfExtent{ collider.extent / 2.0f * transform.scale };
        shape->setGeometry(PxBoxGeometry(PxVec3{ halfExtent.x, halfExtent.y, halfExtent.z }));
        shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !collider.trigger);
        shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, collider.trigger);
    }
}

template<>
void onComponentUpdate<SphereColliderComponent>(entt::registry& registry, entt::entity entity) {
    if (!active) return;
    auto& collider = registry.get<SphereColliderComponent>(entity);
    if (auto shape = reinterpret_cast<PxShape*>(collider.runtimeShape)) {
        auto& transform = registry.get<TransformComponent>(entity);
        float scalar = glm::max(transform.scale.x, transform.scale.y, transform.scale.z);
        float radius = collider.radius * scalar;
        shape->setGeometry(PxSphereGeometry{ radius });
        shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !collider.trigger);
        shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, collider.trigger);
    }
}

template<>
void onComponentUpdate<CapsuleColliderComponent>(entt::registry& registry, entt::entity entity) {
    if (!active) return;
    auto& collider = registry.get<CapsuleColliderComponent>(entity);
    if (auto shape = reinterpret_cast<PxShape*>(collider.runtimeShape)) {
        auto& transform = registry.get<TransformComponent>(entity);
        float scalar = glm::max(transform.scale.x, transform.scale.y, transform.scale.z);
        float radius = collider.radius * scalar;
        float height = collider.height / 2.0f * scalar;
        shape->setGeometry(PxCapsuleGeometry{ radius, height });
        shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !collider.trigger);
        shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, collider.trigger);
    }
}

template<>
void onComponentUpdate<PhysicsMaterialComponent>(entt::registry& registry, entt::entity entity) {
    if (!active) return;
    auto& material = registry.get<PhysicsMaterialComponent>(entity);
    if (auto mat = reinterpret_cast<PxMaterial*>(material.runtimeMaterial)) {
        mat->setDynamicFriction(material.dynamicFriction);
        mat->setStaticFriction(material.staticFriction);
        mat->setRestitution(material.restitution);
        mat->setFrictionCombineMode(me::enum_value<PxCombineMode::Enum>(me::enum_index(material.frictionCombine).value_or(0)));
        mat->setRestitutionCombineMode(me::enum_value<PxCombineMode::Enum>(me::enum_index(material.restitutionCombine).value_or(0)));
    }
}*/