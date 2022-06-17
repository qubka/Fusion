#include "scene.hpp"
#include "components.hpp"

#include "fusion/renderer/systems/mesh_renderer.hpp"
#include "fusion/renderer/systems/grid_renderer.hpp"
#include "fusion/renderer/systems/sky_renderer.hpp"

#include "fusion/renderer/editor_camera.hpp"
#include "fusion/input/input.hpp"

using namespace fe;
using namespace physx;

PxDefaultAllocator Scene::defaultAllocatorCallback{};
PxDefaultErrorCallback Scene::defaultErrorCallback{};
PxFoundation* Scene::foundation{ nullptr };
PxDefaultCpuDispatcher* Scene::dispatcher{ nullptr };
PxPhysics* Scene::physics{ nullptr };
PxPvd* Scene::pvd{ nullptr };

Scene::Scene() : PxSimulationEventCallback{} {
    init();

    // init physx
    if (!foundation) {
        foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocatorCallback, defaultErrorCallback);
        if (!foundation) throw std::runtime_error("Failed to create PhysX foundation!");
        pvd = PxCreatePvd(*foundation);
        PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
        pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
        dispatcher = PxDefaultCpuDispatcherCreate(2);
        PxTolerancesScale toleranceScale;
        toleranceScale.length = 100; // typical length of an object
        toleranceScale.speed = 981; // typical speed of an object, gravity*1s is a reasonable choice
        physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, toleranceScale, true, pvd);
    }

    PxSceneDesc sceneDesc{physics->getTolerancesScale()};
    sceneDesc.gravity = PxVec3{0.0f, -9.81f, 0.0f};
    sceneDesc.cpuDispatcher	= dispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    scene = physics->createScene(sceneDesc);
    scene->setSimulationEventCallback(this);

    PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
    if (pvdClient) {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    defaultMaterial = physics->createMaterial(0.5f, 0.5f, 0.6f);
}

Scene::~Scene() {
    scene->release();
    defaultMaterial->release();
    active = false;
}

Scene::Scene(const Scene& other) : Scene{} {
    registry.assign(other.registry.data(), other.registry.data() + other.registry.size(), other.registry.released());
    clone<TagComponent>(other.registry);
    clone<RelationshipComponent>(other.registry);
    clone<TransformComponent>(other.registry);
    clone<MeshComponent>(other.registry);
    clone<CameraComponent>(other.registry);
    clone<RigidbodyComponent>(other.registry);
    clone<BoxColliderComponent>(other.registry);
    clone<PhysicsMaterialComponent>(other.registry);
    clone<MaterialComponent>(other.registry);
}

void Scene::onViewportResize(const glm::vec2& size) {

}

void Scene::onRuntimeStart() {
    active = true;

    auto view = registry.view<const TransformComponent, RigidbodyComponent>();
    for (auto [entity, transform, rigidbody] : view.each()) {
        PxTransform t{
            PxVec3{ transform.position.z, transform.position.y, transform.position.z },
            PxQuat{ transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w }
        };

        PxRigidActor* body;
        if (rigidbody.type == RigidbodyComponent::BodyType::Dynamic) {
            auto rigid = physics->createRigidDynamic(t);
            rigid->setMass(rigidbody.mass);
            rigid->setLinearDamping(rigidbody.linearDrag);
            rigid->setAngularDamping(rigidbody.angularDrag);
            rigid->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, rigidbody.kinematic);
            rigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, rigidbody.freezePosition.x);
            rigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, rigidbody.freezePosition.y);
            rigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, rigidbody.freezePosition.z);
            rigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, rigidbody.freezeRotation.x);
            rigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, rigidbody.freezeRotation.y);
            rigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, rigidbody.freezeRotation.z);

            body = rigid;
        } else {
            body = physics->createRigidStatic(t);
        }

        float scalar = glm::max(transform.scale.x, transform.scale.y, transform.scale.z);

        PxMaterial* mat;
        if (auto material = registry.try_get<PhysicsMaterialComponent>(entity)) {
            mat = physics->createMaterial(material->staticFriction, material->dynamicFriction, material->restitution);
            mat->setRestitutionCombineMode(me::enum_value<PxCombineMode::Enum>(me::enum_index(material->restitutionCombine).value_or(0)));
            mat->setFrictionCombineMode(me::enum_value<PxCombineMode::Enum>(me::enum_index(material->frictionCombine).value_or(0)));
            material->runtimeMaterial = mat;
        } else {
            mat = defaultMaterial;
        }

        if (auto collider = registry.try_get<BoxColliderComponent>(entity)) {
            glm::vec3 halfExtent{ collider->extent / 2.0f * transform.scale };

            PxShape* shape = physics->createShape(PxBoxGeometry{{ halfExtent.x, halfExtent.y, halfExtent.z }}, *mat);
            if (collider->trigger) {
                shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
                shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
            }
            body->attachShape(*shape);
            collider->runtimeShape = shape;
        }

        if (auto collider = registry.try_get<SphereColliderComponent>(entity)) {
            float radius = collider->radius * scalar;

            PxShape* shape = physics->createShape(PxSphereGeometry{ radius }, *mat);
            if (collider->trigger) {
                shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
                shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
            }
            body->attachShape(*shape);
            collider->runtimeShape = shape;
        }

        if (auto collider = registry.try_get<CapsuleColliderComponent>(entity)) {
            float radius = collider->radius * scalar;
            float height = collider->height / 2.0f * scalar;

            PxShape* shape = physics->createShape(PxCapsuleGeometry{ radius, height }, *mat);
            if (collider->trigger) {
                shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
                shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
            }
            body->attachShape(*shape);
            collider->runtimeShape = shape;
        }

        /*if (auto collider = world.try_get<MeshColliderComponent>(entity)) {

        }*/

        rigidbody.runtimeBody = body;
        scene->addActor(*body);
    }
}

void Scene::onRuntimeStop() {
    active = false;
}

void Scene::onUpdateRuntime(float dt) {
    // Physics
    {
        scene->simulate(dt);
        scene->fetchResults(true);

        // Retrieve transform from PhysX
        auto view = registry.view<TransformComponent, const RigidbodyComponent>();
        for (auto [entity, transform, rigidbody] : view.each()) {
            if (rigidbody.type == RigidbodyComponent::BodyType::Static)
                continue;
            auto body = reinterpret_cast<PxRigidBody*>(rigidbody.runtimeBody);
            PxTransform t{ body->getGlobalPose() };
            transform.position = { t.p.x, t.p.y, t.p.z };
            transform.rotation = { t.q.w, t.q.x, t.q.y, t.q.z };
            registry.patch<TransformComponent>(entity);
        }
    }
}

void Scene::onUpdateEditor(float dt) {

}

void Scene::onRenderRuntime() {
    render();
}

void Scene::onRenderEditor(const EditorCamera& camera) {
    render();

    auto& skyRenderer = SkyRenderer::Instance();
    skyRenderer.begin();
    skyRenderer.draw();
    skyRenderer.end();

    auto& gridRenderer = GridRenderer::Instance();
    gridRenderer.begin();
    gridRenderer.draw();
    gridRenderer.end();
}

glm::mat4 getTRS(const entt::registry& registry, const entt::entity entity) {
    if (auto component = registry.try_get<RelationshipComponent>(entity); component && component->parent != entt::null) {
        return registry.get<TransformComponent>(entity).transform() * getTRS(registry, component->parent);
    } else {
        return registry.get<TransformComponent>(entity).transform();
    }
}

void Scene::render() {
    auto transformGroup = registry.group<DirtyTransformComponent, TransformComponent>();
    transformGroup.sort([&](const entt::entity lhs, const entt::entity rhs) {
        auto clhs = registry.try_get<RelationshipComponent>(lhs);
        if (clhs == nullptr)
            return false;
        auto crhs = registry.try_get<RelationshipComponent>(rhs);
        if (crhs == nullptr)
            return false;
        return !(clhs->parent != entt::null && clhs->children < crhs->children);
    });
    for (auto [entity, transform] : transformGroup.each()) {
        transform.model = getTRS(registry, entity);
    }

    registry.clear<DirtyTransformComponent>();

    auto& meshRenderer = MeshRenderer::Instance();

    auto meshView = registry.view<DirtyMeshComponent, MeshComponent>();
    for (auto [entity, mesh] : meshView.each()) {
        if (!mesh.path.empty() && std::filesystem::exists(mesh.path))
            mesh.runtimeModel = meshRenderer.loadModel(mesh.path);
        else
            mesh.runtimeModel = nullptr;
    }

    registry.clear<DirtyMeshComponent>();

    meshRenderer.begin();

    auto transformView = registry.view<const TransformComponent, const MeshComponent>();
    for (auto [entity, transform, mesh] : transformView.each()) {
        if (auto model = reinterpret_cast<vkx::Model*>(mesh.runtimeModel))
            meshRenderer.draw(*model, transform.model);
    }

    meshRenderer.end();
}

template<typename Component>
void Scene::onComponentConstruct(entt::registry& registry, entt::entity entity) {
}

template<typename Component>
void Scene::onComponentUpdate(entt::registry& registry, entt::entity entity) {
}

template<typename Component>
void Scene::onComponentDestroy(entt::registry& registry, entt::entity entity) {
}

template<>
void Scene::onComponentUpdate<RigidbodyComponent>(entt::registry& registry, entt::entity entity) {
    if (!active) return;
    auto& rigidbody = registry.get<RigidbodyComponent>(entity);
    if (rigidbody.type == RigidbodyComponent::BodyType::Dynamic) {
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
void Scene::onComponentDestroy<RigidbodyComponent>(entt::registry& registry, entt::entity entity) {
    if (!active) return;
    auto& rigidbody = registry.get<RigidbodyComponent>(entity);
    if (auto body = reinterpret_cast<PxRigidActor*>(rigidbody.runtimeBody)) {
        scene->removeActor(*body);
    }
}

template<>
void Scene::onComponentUpdate<BoxColliderComponent>(entt::registry& registry, entt::entity entity) {
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
void Scene::onComponentUpdate<SphereColliderComponent>(entt::registry& registry, entt::entity entity) {
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
void Scene::onComponentUpdate<CapsuleColliderComponent>(entt::registry& registry, entt::entity entity) {
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
void Scene::onComponentUpdate<PhysicsMaterialComponent>(entt::registry& registry, entt::entity entity) {
    if (!active) return;
    auto& material = registry.get<PhysicsMaterialComponent>(entity);
    if (auto mat = reinterpret_cast<PxMaterial*>(material.runtimeMaterial)) {
        mat->setDynamicFriction(material.dynamicFriction);
        mat->setStaticFriction(material.staticFriction);
        mat->setRestitution(material.restitution);
        mat->setFrictionCombineMode(me::enum_value<PxCombineMode::Enum>(me::enum_index(material.frictionCombine).value_or(0)));
        mat->setRestitutionCombineMode(me::enum_value<PxCombineMode::Enum>(me::enum_index(material.restitutionCombine).value_or(0)));
    }
}

void Scene::init() {
    registry.on_construct<TransformComponent>().connect<&entt::registry::emplace<DirtyTransformComponent>>();
    registry.on_update<TransformComponent>().connect<&entt::registry::emplace_or_replace<DirtyTransformComponent>>();
    registry.on_destroy<TransformComponent>().connect<&entt::registry::remove<DirtyTransformComponent>>();

    //
    registry.on_construct<MeshComponent>().connect<&entt::registry::emplace<DirtyMeshComponent>>();
    registry.on_update<MeshComponent>().connect<&entt::registry::emplace_or_replace<DirtyMeshComponent>>();
    registry.on_destroy<MeshComponent>().connect<&entt::registry::remove<DirtyMeshComponent>>();

    //

    registry.on_update<RigidbodyComponent>().connect<&Scene::onComponentUpdate<RigidbodyComponent>>(this);
    registry.on_destroy<RigidbodyComponent>().connect<&Scene::onComponentDestroy<RigidbodyComponent>>(this);

    //
    registry.on_update<BoxColliderComponent>().connect<&Scene::onComponentUpdate<BoxColliderComponent>>(this);
    registry.on_update<SphereColliderComponent>().connect<&Scene::onComponentUpdate<SphereColliderComponent>>(this);
    registry.on_update<CapsuleColliderComponent>().connect<&Scene::onComponentUpdate<CapsuleColliderComponent>>(this);

    registry.on_update<PhysicsMaterialComponent>().connect<&Scene::onComponentUpdate<PhysicsMaterialComponent>>(this);
}