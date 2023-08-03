# Used to track if we're using ONLY system libs
# Prevents errors with EXPORT
set(ALL_LIBS)

# vulkan
#╒◖═════════════════════════════════════════════════════════════════════◗╕
if(ANDROID)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_ANDROID_KHR)
elseif(WIN32)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
else()
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_XCB_KHR)
endif()
add_subdirectory(volk)
list(APPEND ALL_LIBS volk)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# tracy
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(tracy QUIET)
if(NOT tracy_FOUND)
    add_subdirectory(tracy)
endif()
list(APPEND ALL_LIBS TracyClient)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# glfw
#╒◖═════════════════════════════════════════════════════════════════════◗╕
if(NOT ANDROID)
    find_package(glfw QUIET)
    if(NOT glfw_FOUND)
        foreach(_glfw_option "GLFW_BUILD_TESTS" "GLFW_BUILD_EXAMPLES" "GLFW_BUILD_DOCS" "GLFW_INSTALL")
            set(${_glfw_option} OFF CACHE INTERNAL "")
        endforeach()

        add_subdirectory(glfw)
    endif()
    list(APPEND ALL_LIBS glfw)
endif()
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# plog
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(plog QUIET)
if(NOT plog_FOUND)
    set(PLOG_BUILD_SAMPLES OFF CACHE INTERNAL "")
    add_subdirectory(plog)
endif()
list(APPEND ALL_LIBS plog)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# span
#╒◖═════════════════════════════════════════════════════════════════════◗╕
if(WIN32)
    find_package(GSL QUIET)
    if(NOT GSL_FOUND)
        add_library(gsl INTERFACE)

        target_include_directories(gsl INTERFACE gsl/include)
    endif()
    list(APPEND ALL_LIBS gsl)
else() # May be remove ?
    find_package(span QUIET)
    if(NOT span_FOUND)
        add_library(span INTERFACE)

        target_include_directories(span INTERFACE span/include)
    endif()
    list(APPEND ALL_LIBS span)
endif()
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# glm
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(glm QUIET)
if(NOT glm_FOUND)
    add_library(glm INTERFACE)

    target_include_directories(glm INTERFACE glm/)
    target_compile_definitions(glm INTERFACE GLM_FORCE_DEPTH_ZERO_TO_ONE)
endif()
list(APPEND ALL_LIBS glm)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# gli
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(gli QUIET)
if(NOT gli_FOUND)
    add_library(gli INTERFACE)

    target_include_directories(gli INTERFACE gli/)
endif()
list(APPEND ALL_LIBS gli)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# entt
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(entt QUIET)
if(NOT entt_FOUND)
    add_library(entt INTERFACE)

    target_include_directories(entt INTERFACE entt/src)
endif()
list(APPEND ALL_LIBS entt)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# bitmask
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(bitmask QUIET)
if(NOT bitmask_FOUND)
    add_library(bitmask INTERFACE)

    target_include_directories(bitmask INTERFACE bitmask/include)
endif()
list(APPEND ALL_LIBS bitmask)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# magic_enum
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(magic_enum QUIET)
if(NOT magic_enum_FOUND)
    add_library(magic_enum INTERFACE)

    target_include_directories(magic_enum INTERFACE magic_enum/include)
endif()
list(APPEND ALL_LIBS magic_enum)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# stb
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(Stb QUIET)
if(NOT Stb_FOUND)
    add_library(stb INTERFACE)

    target_include_directories(stb INTERFACE stb/)
endif()
list(APPEND ALL_LIBS stb)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# freetype
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(Freetype QUIET)
if(NOT Freetype_FOUND)
    set(FT_DISABLE_ZLIB TRUE CACHE INTERNAL "")
    set(FT_DISABLE_BZIP2 TRUE CACHE INTERNAL "")
    set(FT_DISABLE_PNG TRUE CACHE INTERNAL "")
    set(FT_DISABLE_HARFBUZZ TRUE CACHE INTERNAL "")

    add_subdirectory(freetype)
endif()
list(APPEND ALL_LIBS freetype)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# random
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(randomQUIET)
if(NOT random_FOUND)
    add_subdirectory(random)
endif()
list(APPEND ALL_LIBS effolkronium_random)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# meshoptimizer
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(meshoptimizer QUIET)
if(NOT meshoptimizer_FOUND)
    add_subdirectory(meshoptimizer)
endif()
list(APPEND ALL_LIBS meshoptimizer)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# glslang
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(glslang QUIET)
if(NOT glslang_FOUND)
    set(GLSLANG_LIB_NAME "glslang")

    # On MSVC shared mode must be disabled with glslang currently
    set(BUILD_SHARED_LIBS_SAVED "${BUILD_SHARED_LIBS}")
    if(MSVC)
        set(BUILD_SHARED_LIBS OFF)
    endif()
    set(ENABLE_SPVREMAPPER OFF CACHE INTERNAL "Enables building of SPVRemapper")
    set(ENABLE_GLSLANG_BINARIES OFF CACHE INTERNAL "Builds glslangValidator and spirv-remap")
    set(ENABLE_HLSL OFF CACHE INTERNAL "Enables HLSL input support")
    set(ENABLE_RTTI ON CACHE INTERNAL "Enables RTTI")
    set(ENABLE_EXCEPTIONS ON CACHE INTERNAL "Enables Exceptions")
    set(ENABLE_CTEST OFF CACHE INTERNAL "Enables testing")
    set(SKIP_GLSLANG_INSTALL ON CACHE INTERNAL "Skip installation")

    add_subdirectory(${GLSLANG_LIB_NAME})

    # Reset back to value before MSVC fix
    set(BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS_SAVED}")

    set_target_properties(glslang PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
    #set_target_properties(glslang-build-info PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
    set_target_properties(GenericCodeGen PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
    set_target_properties(MachineIndependent PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
    set_target_properties(OGLCompiler PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
    set_target_properties(OSDependent PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
    set_target_properties(SPIRV PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
endif()
list(APPEND ALL_LIBS glslang SPIRV)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# cereal
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(cereal QUIET)
if(NOT cereal_FOUND)
    set(BUILD_DOC OFF CACHE INTERNAL "")
    set(BUILD_SANDBOX OFF CACHE INTERNAL "")
    set(SKIP_PERFORMANCE_COMPARISON ON CACHE INTERNAL "")

    add_subdirectory(cereal)
endif()
list(APPEND ALL_LIBS cereal)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# mono
#╒◖═════════════════════════════════════════════════════════════════════◗╕
if(NOT ANDROID)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(MONO REQUIRED mono-2)
	list(APPEND ALL_LIBS ${MONO_LIBRARIES})
	list(APPEND ALL_DIRS ${MONO_LIBRARY_DIRS})
	list(APPEND ALL_INCS ${MONO_INCLUDE_DIRS})
endif()
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# physx
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#set(PHYSX_LIBRARIES
#            PhysXExtensions
#            PhysX
#            PhysXPvdSDK
#            PhysXVehicle
#            PhysXCharacterKinematic
#            PhysXCooking
#            PhysXCommon
#            PhysXFoundation
#            # SnippetUtils
#            )

#list(APPEND ALL_LIBS ${PHYSX_LIBRARIES})

#find_package(PhysX 4.1.1 QUIET COMPONENTS PHYSX_LIBRARIES)

#╘◖═════════════════════════════════════════════════════════════════════◗╛

# physfs
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(PhysFS QUIET)
if(NOT PhysFS_FOUND)
    if(BUILD_SHARED_LIBS)
        set(PHYSFS_BUILD_STATIC OFF CACHE INTERNAL "Build static library.")
    else()
        set(PHYSFS_BUILD_SHARED OFF CACHE INTERNAL "Build shared library.")
    endif()
    set(PHYSFS_BUILD_TEST OFF CACHE INTERNAL "Build stdio test program.")
    set(PHYSFS_BUILD_DOCS OFF CACHE INTERNAL "Build doxygen based documentation.")
    set(PHYSFS_TARGETNAME_DIST "physfs-dist" CACHE INTERNAL STRING)
    set(PHYSFS_TARGETNAME_UNINSTALL "physfs-uninstall" CACHE INTERNAL STRING)

    add_subdirectory(physfs)
endif()
list(APPEND ALL_LIBS physfs-static)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# lz4
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(LZ4 QUIET)
#if(NOT LZ4_FOUND)
#    add_subdirectory(lz4/build/cmake/)
#endif()
#list(APPEND ALL_LIBS lz4)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

#assimp
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(assimp QUIET)
if(NOT assimp_FOUND)
    include_directories(assimp/include)
    add_subdirectory(assimp)
endif()
list(APPEND ALL_LIBS assimp)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# imgui and plugins
#╒◖═════════════════════════════════════════════════════════════════════◗╕
set(IMGUI_SOURCES
        "imgui/imgui.cpp"
        "imgui/imgui_demo.cpp"
        "imgui/imgui_draw.cpp"
        "imgui/imgui_tables.cpp"
        "imgui/imgui_widgets.cpp"
        "imguizmo/GraphEditor.cpp"
        "imguizmo/ImCurveEdit.cpp"
        "imguizmo/ImGradient.cpp"
        "imguizmo/ImGuizmo.cpp"
        "imguizmo/ImSequencer.cpp"

        "imgui-plugins/ImTextEditor.cpp"
        )

set(IMGUI_HEADERS
        "imgui/imconfig.h"
        "imgui/imgui.h"
        "imgui/imgui_internal.h"
        #"imgui/imstb_rectpack.h"
        #"imgui/imstb_textedit.h"
        #"imgui/imstb_truetype.h"

        "imgui-plugins/ImFileBrowser.h"
        "imgui-plugins/ImTextEditor.h"

        "imguizmo/GraphEditor.h"
        "imguizmo/ImCurveEdit.h"
        "imguizmo/ImGradient.h"
        "imguizmo/ImGuizmo.h"
        "imguizmo/ImSequencer.h"
        "imguizmo/ImZoomSlider.h"
        )

add_library(imgui
        STATIC
        ${IMGUI_SOURCES}
        ${IMGUI_HEADERS}
        )

target_compile_definitions(imgui
        PRIVATE
        #IMGUI_IMPL_OPENGL_LOADER_GLAD=1
        USE_IMGUI_API
        )

target_include_directories(imgui
        PUBLIC
        "imgui/"
        "imguizmo/"
        "imgui-plugins/"
        "${GLFW_INC_PATH}"
        #"${GLAD_INC_PATH}"
        )

list(APPEND ALL_LIBS imgui)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

set(FUSION_LINK_LIBS ${ALL_LIBS} CACHE INTERNAL "")
set(FUSION_LIBRARY_DIRS ${ALL_DIRS} CACHE INTERNAL "")
set(FUSION_INCLUDE_DIRS ${ALL_INCS} CACHE INTERNAL "")
