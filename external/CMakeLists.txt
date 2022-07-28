# Used to track if we're using ONLY system libs
# Prevents errors with EXPORT
set(ALL_SYSTEM_LIBS)

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
list(APPEND ALL_SYSTEM_LIBS volk)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# tracy
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(tracy 0.8.2 QUIET)
if(NOT tracy_FOUND)
    add_subdirectory(tracy)
endif()
list(APPEND ALL_SYSTEM_LIBS TracyClient)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# glfw
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(glfw3 3.3.6 QUIET)
if(NOT glfw_FOUND)
    foreach(_glfw3_option "GLFW_BUILD_TESTS" "GLFW_BUILD_EXAMPLES" "GLFW_BUILD_DOCS" "GLFW_INSTALL")
        set(${_glfw3_option} OFF CACHE INTERNAL "")
    endforeach()

    add_subdirectory(glfw)
endif()
list(APPEND ALL_SYSTEM_LIBS glfw)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# plog
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(plog 1.1.8 QUIET)
if(NOT plog_FOUND)
    set(PLOG_BUILD_SAMPLES OFF CACHE INTERNAL "")
    add_subdirectory(plog)
endif()
list(APPEND ALL_SYSTEM_LIBS plog)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# span
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(span QUIET)
if(NOT span_FOUND)
    add_library(span INTERFACE)

    target_include_directories(span INTERFACE span/include)
endif()
list(APPEND ALL_SYSTEM_LIBS span)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# stduuid
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(stduuid 1.2.2 QUIET)
if(NOT stduuid_FOUND)
    add_library(stduuid INTERFACE)

    target_include_directories(stduuid INTERFACE stduuid/include)
endif()
list(APPEND ALL_SYSTEM_LIBS stduuid)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# glm
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(glm 0.9.9 QUIET)
if(NOT glm_FOUND)
    add_library(glm INTERFACE)

    target_include_directories(glm INTERFACE glm/)
    target_compile_definitions(glm INTERFACE GLM_FORCE_DEPTH_ZERO_TO_ONE)
endif()
list(APPEND ALL_SYSTEM_LIBS glm)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# gli
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(gli 0.8.2 QUIET)
if(NOT gli_FOUND)
    add_library(gli INTERFACE)

    target_include_directories(gli INTERFACE gli/)
endif()
list(APPEND ALL_SYSTEM_LIBS gli)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# entt
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(entt 3.9.0 QUIET)
if(NOT entt_FOUND)
    add_library(entt INTERFACE)

    target_include_directories(entt INTERFACE entt/src)
endif()
list(APPEND ALL_SYSTEM_LIBS entt)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# bitmask
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(bitmask 0.10.2 QUIET)
if(NOT bitmask_FOUND)
    add_library(bitmask INTERFACE)

    target_include_directories(bitmask INTERFACE bitmask/include)
endif()
list(APPEND ALL_SYSTEM_LIBS bitmask)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# magic_enum
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(magic_enum 0.7.3 QUIET)
if(NOT magic_enum_FOUND)
    add_library(magic_enum INTERFACE)

    target_include_directories(magic_enum INTERFACE magic_enum/include)
endif()
list(APPEND ALL_SYSTEM_LIBS magic_enum)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# stb
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(stb QUIET)
if(NOT stb_FOUND)
    add_library(stb INTERFACE)

    target_include_directories(stb INTERFACE stb/)
endif()
list(APPEND ALL_SYSTEM_LIBS stb)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# freetype
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(Freetype 2.11.1 QUIET)
if(NOT Freetype_FOUND)
    set(FT_DISABLE_ZLIB TRUE CACHE INTERNAL "")
    set(FT_DISABLE_BZIP2 TRUE CACHE INTERNAL "")
    set(FT_DISABLE_PNG TRUE CACHE INTERNAL "")
    set(FT_DISABLE_HARFBUZZ TRUE CACHE INTERNAL "")

    add_subdirectory(freetype)
endif()
list(APPEND ALL_SYSTEM_LIBS freetype)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# random
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(random 0.0.0 QUIET)
if(NOT random_FOUND)
    add_subdirectory(random)
endif()
list(APPEND ALL_SYSTEM_LIBS effolkronium_random)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# meshoptimizer
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(meshoptimizer 0.17 QUIET)
if(NOT meshoptimizer_FOUND)
    add_subdirectory(meshoptimizer)
endif()
list(APPEND ALL_SYSTEM_LIBS meshoptimizer)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# glslang
#╒◖═════════════════════════════════════════════════════════════════════◗╕
##find_package(glslang 11.6.0 QUIET)
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
list(APPEND ALL_SYSTEM_LIBS glslang SPIRV)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# cereal
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(cereal 1.3.2 QUIET)
if(NOT cereal_FOUND)
    set(BUILD_DOC OFF CACHE INTERNAL "")
    set(BUILD_SANDBOX OFF CACHE INTERNAL "")
    set(SKIP_PERFORMANCE_COMPARISON ON CACHE INTERNAL "")

    add_subdirectory(cereal)
endif()
list(APPEND ALL_SYSTEM_LIBS cereal)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# physx
#╒◖═════════════════════════════════════════════════════════════════════◗╕
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

list(APPEND ALL_SYSTEM_LIBS ${PHYSX_LIBRARIES})

#find_package(PhysX 4.1.1 QUIET COMPONENTS PHYSX_LIBRARIES)
if(NOT PhysX_FOUND)
    # SnippetUtils
    # PHYSX_ROOT_DIR - path to the `{cloned repository}/physx` repo directory git://github.com/NVIDIAGameWorks/PhysX.git
    set(PHYSX_ROOT_DIR ${CMAKE_SOURCE_DIR}/external/physx/physx) #This is needed for ${PHYSX_ROOT_DIR}/compiler/public/CMakeLists.txt
    set(PHYSX_INCLUDE_DIRS ${PHYSX_ROOT_DIR}/include/ $ENV{PHYSX_ROOT_DIR}/../pxshared/include/)

    # has to match the TARGET_BUILD_PLATFORM in ${PHYSX_ROOT_DIR}/physix/buildtools/cmake_generate_projects.py
    if (ANDROID)
        set(TARGET_BUILD_PLATFORM "android")
    elseif (WIN32)
        set(TARGET_BUILD_PLATFORM "windows")
    elseif (APPLE)
        set(TARGET_BUILD_PLATFORM "mac")
    elseif (IOS)
        set(TARGET_BUILD_PLATFORM "ios")
    else()
        set(TARGET_BUILD_PLATFORM "linux")
    endif()
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-error=restrict -Wno-error=mismatched-new-delete")
    set(PX_BUILDSNIPPETS OFF CACHE BOOL "Generate the snippets")
    set(PX_BUILDPUBLICSAMPLES OFF CACHE BOOL "Generate the samples projects")
    set(PX_GENERATE_STATIC_LIBRARIES ON CACHE BOOL "Generate static libraries")
    set(PX_FLOAT_POINT_PRECISE_MATH OFF CACHE BOOL "Float point precise math")
    set(NV_USE_STATIC_WINCRT ON CACHE BOOL "Use the statically linked windows CRT")
    set(NV_USE_DEBUG_WINCRT ON CACHE BOOL "Use the debug version of the CRT")
    set(PXSHARED_PATH ${PHYSX_ROOT_DIR}/../pxshared)
    set(PXSHARED_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
    set(CMAKEMODULES_VERSION "1.27")
    set(CMAKEMODULES_PATH ${PHYSX_ROOT_DIR}/../externals/cmakemodules)
    set(PX_OUTPUT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/physx)
    set(PX_OUTPUT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/physx)

    # Call into PhysX's CMake scripts
    add_subdirectory(${PHYSX_ROOT_DIR}/compiler/public/)

    # Reset back to value before MSVC fix
    set(BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS_SAVED}")

    # Add physx libraries to target
    #target_link_libraries(target_name PUBLIC ${PHYSX_LIBRARIES})
    #### Windows only: Copy the Physx dll files to the simulation executable####

    if (TARGET_BUILD_PLATFORM STREQUAL "windows")
        # References NvidiaBuildOptions.cmake to figure out if system is 32/64 bit
        IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
            SET(LIBPATH_SUFFIX "64")
        ELSE()
            SET(LIBPATH_SUFFIX "32")
        ENDIF()
        GetPlatformBinName(PLATFORM_BIN_NAME ${LIBPATH_SUFFIX})
        set(PhysxOutputPath ${PX_OUTPUT_LIB_DIR}/bin/${PLATFORM_BIN_NAME}/)
        message("Physx Output Path: " ${PhysxOutputPath})

        # copy PhysX dll's to build dir. Happens on every build.
        add_custom_command(TARGET target_name POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${PhysxOutputPath}" "$<TARGET_FILE_DIR:target_name >/..")
    endif()

endif()
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# physfs
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(PhysFS 3.0.2 QUIET)
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
list(APPEND ALL_SYSTEM_LIBS physfs-static)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# lz4
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(LZ4 1.9.3 QUIET)
if(NOT LZ4_FOUND)
    add_subdirectory(lz4/build/cmake/)
endif()
list(APPEND ALL_SYSTEM_LIBS lz4)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

#assimp
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(assimp REQUIRED)
if(NOT assimp_FOUND)
    include_directories(assimp/include)
    add_subdirectory(assimp)
endif()
list(APPEND ALL_SYSTEM_LIBS assimp)
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

list(APPEND ALL_SYSTEM_LIBS imgui)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

set(FUSION_ALL_SYSTEM_LIBS ${ALL_SYSTEM_LIBS} CACHE INTERNAL "")
