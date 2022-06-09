#pragma once

#include "world.hpp"

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

namespace fe {
    class EditorCamera;
    class Renderer;

    class Scene {
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

        entt::world world;

    private:
        void render();

        // physics
        static physx::PxDefaultAllocator defaultAllocatorCallback;
        static physx::PxDefaultErrorCallback defaultErrorCallback;

        static physx::PxFoundation* foundation;
        static physx::PxDefaultCpuDispatcher* dispatcher;
        static physx::PxPvd* pvd;
        static physx::PxPhysics* physics;

        physx::PxScene* scene;
        physx::PxMaterial* defaultMaterial;
    };
}
