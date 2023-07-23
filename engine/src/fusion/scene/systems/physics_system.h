#pragma once

#include "fusion/scene/system.h"
#include "fusion/scene/components.h"

//#include <PxPhysics.h>
//#include <PxPhysicsAPI.h>

namespace fe {
    class PhysicsSystem final : public System/*, public physx::PxSimulationEventCallback*/ {
    public:
        explicit PhysicsSystem(entt::registry& registry);
        ~PhysicsSystem() override;

    private:
        void onPlay() override;
        void onUpdate() override;
        void onStop() override;
        void onEnabled() override {};
        void onDisabled() override {};

        // connect to events
       /* void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override {};
        void onWake(physx::PxActor** actors, physx::PxU32 count) override {};
        void onSleep(physx::PxActor** actors, physx::PxU32 count) override {};
        void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override {};
        void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override {};
        void onAdvance(const physx::PxRigidBody* const * bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override {};

        // physics
        static physx::PxDefaultAllocator defaultAllocatorCallback;
        static physx::PxDefaultErrorCallback defaultErrorCallback;
        static physx::PxFoundation* foundation;
        static physx::PxDefaultCpuDispatcher* dispatcher;
        static physx::PxPvd* pvd;
        static physx::PxPhysics* physics;

        physx::PxScene* scene;
        physx::PxMaterial* defaultMaterial;*/
    };
}
