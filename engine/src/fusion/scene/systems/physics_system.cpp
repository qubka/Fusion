#include "physics_system.hpp"

#include "fusion/scene/scene_manager.hpp"

using namespace fe;
using namespace physx;

PxDefaultAllocator PhysicsSystem::defaultAllocatorCallback{};
PxDefaultErrorCallback PhysicsSystem::defaultErrorCallback{};
PxFoundation* PhysicsSystem::foundation = nullptr;
PxDefaultCpuDispatcher* PhysicsSystem::dispatcher = nullptr;
PxPhysics* PhysicsSystem::physics = nullptr;
PxPvd* PhysicsSystem::pvd = nullptr;

PhysicsSystem::PhysicsSystem(entt::registry& registry) : System{registry} {
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

PhysicsSystem::~PhysicsSystem() {
    scene->release();
    defaultMaterial->release();
}

void PhysicsSystem::onPlay() {
    /*auto view = registry.view<TransformComponent, RigidbodyComponent>();
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
        }

        rigidbody.runtimeBody = body;
        scene->addActor(*body);
    }*/
}

void PhysicsSystem::onStop() {

}

void PhysicsSystem::onUpdate() {
    if (!SceneManager::Get()->getScene()->isRuntime())
        return;

    // Physics
    {
        scene->simulate(Time::DeltaTime().asSeconds());
        scene->fetchResults(true);

        // Retrieve transform from PhysX
        /*auto view = registry.view<TransformComponent, const RigidbodyComponent>();
        for (auto [entity, transform, rigidbody] : view.each()) {
            if (rigidbody.type == RigidbodyComponent::BodyType::Static)
                continue;
            auto body = reinterpret_cast<PxRigidBody*>(rigidbody.runtimeBody);
            PxTransform t{ body->getGlobalPose() };
            transform.position = { t.p.x, t.p.y, t.p.z };
            transform.rotation = { t.q.w, t.q.x, t.q.y, t.q.z };
            registry.patch<TransformComponent>(entity);
        }*/
    }
}
