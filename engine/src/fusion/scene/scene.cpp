#include "scene.hpp"
#include "components.hpp"

#include "fusion/renderer/systems/model_renderer.hpp"
#include "fusion/renderer/systems/grid_renderer.hpp"
#include "fusion/renderer/editor_camera.hpp"
#include "fusion/input/input.hpp"

using namespace fe;

physx::PxDefaultAllocator Scene::defaultAllocatorCallback{};
physx::PxDefaultErrorCallback Scene::defaultErrorCallback{};
physx::PxFoundation* Scene::foundation{ nullptr };
physx::PxDefaultCpuDispatcher* Scene::dispatcher{ nullptr };
physx::PxPhysics* Scene::physics{ nullptr };
physx::PxPvd* Scene::pvd{ nullptr };

Scene::Scene() {

    world.on_construct<TransformComponent>().connect<&entt::registry::emplace<DirtyTransformComponent>>();
    world.on_update<TransformComponent>().connect<&entt::registry::emplace_or_replace<DirtyTransformComponent>>();

    world.on_construct<ModelComponent>().connect<&entt::registry::emplace<DirtyModelComponent>>();
    world.on_update<ModelComponent>().connect<&entt::registry::emplace_or_replace<DirtyModelComponent>>();

    world.on_destroy<RigidbodyComponent>().connect<>();
    world.on_destroy<PhysicsMaterialComponent>().connect<>();

    // init physx
    if (!foundation) {
        foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocatorCallback, defaultErrorCallback);
        if (!foundation) throw std::runtime_error("Failed to create PhysX foundation!");
        pvd = PxCreatePvd(*foundation);
        physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
        pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
        dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
        physx::PxTolerancesScale toleranceScale;
        toleranceScale.length = 100; // typical length of an object
        toleranceScale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice
        physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, toleranceScale, true, pvd);
    }

    physx::PxSceneDesc sceneDesc{physics->getTolerancesScale()};
    sceneDesc.gravity = physx::PxVec3{0.0f, -9.81f, 0.0f};
    sceneDesc.cpuDispatcher	= dispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    scene = physics->createScene(sceneDesc);

    physx::PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
    if (pvdClient) {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
}

Scene::~Scene() {
    scene->release();
    defaultMaterial->release();
}

Scene::Scene(const Scene& other) : Scene() {
    world.assign(other.world.data(), other.world.data() + other.world.size(), other.world.released());
    world.clone<TagComponent>(other.world);
    world.clone<RelationshipComponent>(other.world);
    world.clone<TransformComponent>(other.world);
    world.clone<ModelComponent>(other.world);
    world.clone<CameraComponent>(other.world);
    world.clone<RigidbodyComponent>(other.world);
    world.clone<BoxColliderComponent>(other.world);
    world.clone<PhysicsMaterialComponent>(other.world);
}

void Scene::onViewportResize(const glm::vec2& size) {

}

void Scene::onRuntimeStart() {
    defaultMaterial = physics->createMaterial(0.5f, 0.5f, 0.6f);

    auto view = world.view<const TransformComponent, RigidbodyComponent>();
    for (auto [entity, transform, rigidbody] : view.each()) {
        physx::PxTransform t{
            physx::PxVec3{ transform.position.z, transform.position.y, transform.position.z },
            physx::PxQuat{ transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w }
        };

        physx::PxRigidActor* body;
        if (rigidbody.type == RigidbodyComponent::BodyType::Dynamic) {
            auto rigid = physics->createRigidDynamic(t);
            rigid->setMass(rigidbody.mass);
            rigid->setLinearDamping(rigidbody.linearDrag);
            rigid->setAngularDamping(rigidbody.angularDrag);
            body = rigid;
        } else {
            body = physics->createRigidStatic(t);
        }

        if (auto collider = world.try_get<BoxColliderComponent>(entity)) {
            glm::vec3 halfExtent{ collider->size * transform.scale };

            physx::PxMaterial* mat;
            if (auto material = world.try_get<PhysicsMaterialComponent>(entity)) {
                mat = physics->createMaterial(material->friction, material->friction, material->restitution);
                material->runtimeMaterial = mat;
            } else {
                mat = defaultMaterial;
            }

            physx::PxShape* shape = physics->createShape(physx::PxBoxGeometry(physx::PxVec3{ halfExtent.x, halfExtent.y, halfExtent.z }), *mat);
            body->attachShape(*shape);
            rigidbody.runtimeBody = body;
        }

       scene->addActor(*body);
    }
}

void Scene::onRuntimeStop() {

}

void Scene::onUpdateRuntime(float dt) {
    // Physics
    {
        scene->simulate(dt);
        scene->fetchResults(true);

        // Retrieve transform from PhysX
        auto view = world.view<TransformComponent, const RigidbodyComponent>();
        for (auto [entity, transform, rigidbody] : view.each()) {
            if (rigidbody.type == RigidbodyComponent::BodyType::Static)
                continue;
            auto body = reinterpret_cast<physx::PxRigidBody*>(rigidbody.runtimeBody);
            physx::PxTransform t{ body->getGlobalPose() };
            transform.position = { t.p.x, t.p.y, t.p.z };
            transform.rotation = { t.q.w, t.q.x, t.q.y, t.q.z };
            world.patch<TransformComponent>(entity);
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

    auto& gridRenderer = GridRenderer::Instance();
    gridRenderer.begin();
    gridRenderer.draw();
    gridRenderer.end();
}

void Scene::render() {
    auto transformGroup = world.group<DirtyTransformComponent, TransformComponent>();
    transformGroup.sort([&](const entt::entity lhs, const entt::entity rhs) {
        auto clhs = world.try_get<RelationshipComponent>(lhs);
        if (clhs == nullptr)
            return false;
        auto crhs = world.try_get<RelationshipComponent>(rhs);
        if (crhs == nullptr)
            return false;
        return !(clhs->parent != entt::null && clhs->children < crhs->children);
    });
    for (auto [entity, transform] : transformGroup.each()) {
        transform.localToWorldMatrix = world.transform(entity);
        transform.worldToLocalMatrix = glm::inverse(transform.localToWorldMatrix);
    }

    world.clear<DirtyTransformComponent>();

    auto& modelRenderer = ModelRenderer::Instance();

    auto modelView = world.view<DirtyModelComponent, ModelComponent>();
    for (auto [entity, model] : modelView.each()) {
        if (model.path.empty() || !std::filesystem::exists(model.path))
            model.model.reset();
        else
            model.model = modelRenderer.loadModel(model.path);
    }

    world.clear<DirtyModelComponent>();

    modelRenderer.begin();

    auto transformView = world.view<const TransformComponent, const ModelComponent>();
    for (auto [entity, transform, model] : transformView.each()) {
        if (model.model)
            modelRenderer.draw(model.model, transform.localToWorldMatrix);
    }

    modelRenderer.end();
}
