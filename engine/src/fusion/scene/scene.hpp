#pragma once

#include <entt/entity/registry.hpp>

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

namespace fe {
    class EditorCamera;
    class Renderer;

    class Scene : public physx::PxSimulationEventCallback {
    public:
        Scene();
        ~Scene();
        Scene(const Scene& other);

        void onViewportResize(const glm::vec2& size);

        void onRuntimeStart();
        void onRuntimeStop();

        void onUpdateRuntime(float dt);
        void onUpdateEditor(float dt);

        void onRenderRuntime();
        void onRenderEditor(const EditorCamera& camera);

        bool active{ false };
        entt::registry registry;

    private:
        template<typename Component>
        void onComponentConstruct(entt::registry& registry, entt::entity entity);
        template<typename TComponent>
        void onComponentUpdate(entt::registry& registry, entt::entity entity);
        template<typename Component>
        void onComponentDestroy(entt::registry& registry, entt::entity entity);

        template<typename Component>
        void clone(const entt::registry& src) {
            auto view = src.view<const Component>();
            for (auto [entity, component] : view.each()) {
                registry.emplace<Component>(entity, component);
            }
        }

        void init();
        void render();

        // physics
        static physx::PxDefaultAllocator DefaultAllocatorCallback;
        static physx::PxDefaultErrorCallback DefaultErrorCallback;

        static physx::PxFoundation* Foundation;
        static physx::PxDefaultCpuDispatcher* Dispatcher;
        static physx::PxPvd* Pvd;
        static physx::PxPhysics* Physics;

        void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override {};
        void onWake(physx::PxActor** actors, physx::PxU32 count) override {};
        void onSleep(physx::PxActor** actors, physx::PxU32 count) override {};
        void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override {};
        void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override {};
        void onAdvance(const physx::PxRigidBody* const * bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override {};

        physx::PxScene* scene;
        physx::PxMaterial* defaultMaterial;
    };
}
