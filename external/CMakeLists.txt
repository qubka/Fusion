# Used to track if we're using ONLY system libs
# Prevents errors with EXPORT
set(ALL_LIBS)
set(ALL_DIRS)
set(ALL_INCS)

include(_/IncludeVulkan.cmake)
include(_/IncludeTracy.cmake)

if(ANDROID)
    list(APPEND ALL_LIBS game-activity::game-activity_static android log)
else()
    include(_/IncludeGLFW.cmake)
    include(_/IncludePlog.cmake)
    include(_/IncludeMono.cmake)
    include(_/IncludePhysFS.cmake)
endif()

include(_/IncludeFMT.cmake)
include(_/IncludeGLM.cmake)
include(_/IncludeGLI.cmake)
include(_/IncludeEnTT.cmake)
include(_/IncludeBitmask.cmake)
include(_/IncludeMagicEnum.cmake)
include(_/IncludeSTB.cmake)
include(_/IncludeRandom.cmake)
include(_/IncludeSpan.cmake)
include(_/IncludeGLSlang.cmake)
#include(_/IncludeLZ4.cmake)
include(_/IncludeCereal.cmake)
include(_/IncludeMeshOptimizer.cmake)
include(_/IncludeAssimp.cmake)
#include(_/IncludePhysX.cmake)
include(_/IncludeImGui.cmake)
include(_/IncludeLMDB.cmake)
include(_/IncludeUUID.cmake)
include(_/IncludeDrLibs.cmake)

#
include(_/IncludeMSDF.cmake)

set(FUSION_LINK_LIBS ${ALL_LIBS} CACHE INTERNAL "")
set(FUSION_LIBRARY_DIRS ${ALL_DIRS} CACHE INTERNAL "")
set(FUSION_INCLUDE_DIRS ${ALL_INCS} CACHE INTERNAL "")
