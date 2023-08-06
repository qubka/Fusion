set(PHYSX_LIBRARIES
            PhysXExtensions
            PhysX
            PhysXPvdSDK
            PhysXVehicle
            PhysXCharacterKinematic
            PhysXCooking
            PhysXCommon
            PhysXFoundation
            # SnippetUtils
            )

list(APPEND ALL_LIBS ${PHYSX_LIBRARIES})

find_package(PhysX 4.1.1 QUIET COMPONENTS PHYSX_LIBRARIES)