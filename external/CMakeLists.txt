# Used to track if we're using ONLY system libs
# Prevents errors with EXPORT
set(ALL_SYSTEM_LIBS)

# vulkan
#╒◖═════════════════════════════════════════════════════════════════════◗╕
set(VULKAN_LIB_NAME "volk")

if (ANDROID)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_ANDROID_KHR)
elseif (WIN32)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
else()
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_XCB_KHR)
endif()

add_subdirectory(${VULKAN_LIB_NAME})

list(APPEND ALL_SYSTEM_LIBS volk)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# glfw
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(glfw3 3.3.6 QUIET)
if(NOT glfw_FOUND)
    set(GLFW_LIB_NAME "glfw")
    set(GLFW_INC_PATH ${GLFW_LIB_NAME}/include)

    foreach(_glfw3_option "GLFW_BUILD_TESTS" "GLFW_BUILD_EXAMPLES" "GLFW_BUILD_DOCS" "GLFW_INSTALL")
        set(${_glfw3_option} OFF CACHE INTERNAL "")
    endforeach()

    add_subdirectory(${GLFW_LIB_NAME})
endif()
list(APPEND ALL_SYSTEM_LIBS glfw)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# plog
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(plog 1.1.8 QUIET)
if(NOT plog_FOUND)
    set(PLOG_LIB_NAME "plog")

    set(PLOG_BUILD_SAMPLES OFF CACHE INTERNAL "")

    add_subdirectory(${PLOG_LIB_NAME})
endif()
list(APPEND ALL_SYSTEM_LIBS plog)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# glm
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(glm 0.9.9 QUIET)
if(NOT glm_FOUND)
    set(GLM_LIB_NAME "glm")
    set(GLM_INC_PATH ${GLM_LIB_NAME})

    add_library(${GLM_LIB_NAME} INTERFACE)

    target_include_directories(${GLM_LIB_NAME} INTERFACE ${GLM_INC_PATH})
    target_compile_definitions(${GLM_LIB_NAME} INTERFACE GLM_FORCE_DEPTH_ZERO_TO_ONE)
endif()
list(APPEND ALL_SYSTEM_LIBS glm)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# gli
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(gli 0.8.2 QUIET)
if(NOT gli_FOUND)
    set(GLI_LIB_NAME "gli")
    set(GLI_INC_PATH ${GLI_LIB_NAME})

    add_library(${GLI_LIB_NAME} INTERFACE)

    target_include_directories(${GLI_LIB_NAME} INTERFACE ${GLI_INC_PATH})
endif()
list(APPEND ALL_SYSTEM_LIBS gli)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# entt
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(entt 3.9.0 QUIET)
if(NOT entt_FOUND)
    set(ENTT_LIB_NAME "entt")
    set(ENTT_INC_PATH ${ENTT_LIB_NAME}/src)

    add_library(${ENTT_LIB_NAME} INTERFACE)

    target_include_directories(${ENTT_LIB_NAME} INTERFACE ${ENTT_INC_PATH})
endif()
list(APPEND ALL_SYSTEM_LIBS entt)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# bitmask
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(bitmask 1.1.2 QUIET)
if(NOT bitmask_FOUND)
    set(BITMASK_LIB_NAME "bitmask")
    set(BITMASK_INC_PATH ${BITMASK_LIB_NAME}/include)

    add_library(${BITMASK_LIB_NAME} INTERFACE)

    target_include_directories(${BITMASK_LIB_NAME} INTERFACE ${BITMASK_INC_PATH})
endif()
list(APPEND ALL_SYSTEM_LIBS bitmask)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# magic_enum
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(magic_enum 0.7.3 QUIET)
if(NOT magic_enum_FOUND)
    set(MAGIC_ENUM_LIB_NAME "magic_enum")
    set(MAGIC_ENUM_INC_PATH ${MAGIC_ENUM_LIB_NAME}/include)

    add_library(${MAGIC_ENUM_LIB_NAME} INTERFACE)

    target_include_directories(${MAGIC_ENUM_LIB_NAME} INTERFACE ${MAGIC_ENUM_INC_PATH})
endif()
list(APPEND ALL_SYSTEM_LIBS magic_enum)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# stb
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(stb QUIET)
if(NOT stb_FOUND)
    set(STB_LIB_NAME "stb")
    set(STB_INC_PATH ${STB_LIB_NAME}/)

    add_library(${STB_LIB_NAME} INTERFACE)

    target_include_directories(${STB_LIB_NAME} INTERFACE ${STB_INC_PATH})
endif()
list(APPEND ALL_SYSTEM_LIBS stb)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# freetype
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(Freetype 2.11.1 QUIET)
if(NOT Freetype_FOUND)
    set(FREETYPE_LIB_NAME "freetype")

    set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB TRUE)
    set(CMAKE_DISABLE_FIND_PACKAGE_BZip2 TRUE)
    set(CMAKE_DISABLE_FIND_PACKAGE_PNG TRUE)
    set(CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz TRUE)

    add_subdirectory(${FREETYPE_LIB_NAME})
endif()
list(APPEND ALL_SYSTEM_LIBS freetype)
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

# yaml-cpp
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(Yaml-cpp )
if(NOT Yaml-cpp_FOUND)
    set(YAML_LIB_NAME "yaml-cpp")

    set(YAML_CPP_BUILD_CONTRIB OFF CACHE INTERNAL "")
    set(YAML_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

    add_subdirectory(${YAML_LIB_NAME})
endif()
#list(APPEND ALL_SYSTEM_LIBS yaml-cpp)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# cereal
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(cereal)
if(NOT cereal_FOUND)
    set(CEREAL_LIB_NAME "cereal")

    set(BUILD_DOC OFF CACHE INTERNAL "")
    set(BUILD_SANDBOX OFF CACHE INTERNAL "")
    set(SKIP_PERFORMANCE_COMPARISON ON CACHE INTERNAL "")

    add_subdirectory(${CEREAL_LIB_NAME})
endif()
list(APPEND ALL_SYSTEM_LIBS cereal)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# physfs
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(PhysFS 3.0.2 QUIET)
if(NOT PhysFS_FOUND)
    set(PHYSFS_LIB_NAME "physfs")

    if(BUILD_SHARED_LIBS)
        set(PHYSFS_BUILD_STATIC OFF CACHE INTERNAL "Build static library.")
    else()
        set(PHYSFS_BUILD_SHARED OFF CACHE INTERNAL "Build shared library.")
    endif()
    set(PHYSFS_BUILD_TEST OFF CACHE INTERNAL "Build stdio test program.")
    set(PHYSFS_BUILD_DOCS OFF CACHE INTERNAL "Build doxygen based documentation.")
    set(PHYSFS_TARGETNAME_DIST "physfs-dist" CACHE INTERNAL STRING)
    set(PHYSFS_TARGETNAME_UNINSTALL "physfs-uninstall" CACHE INTERNAL STRING)

    add_subdirectory(${PHYSFS_LIB_NAME})

endif()
list(APPEND ALL_SYSTEM_LIBS physfs-static)
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

# portfiledialog
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(portable_file_dialogs QUIET)
if(NOT portable_file_dialogs_FOUND)
    set(PFD_LIB_NAME "portable-file-dialogs")

    add_subdirectory(${PFD_LIB_NAME})
endif()
list(APPEND ALL_SYSTEM_LIBS portable_file_dialogs)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# lz4
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(LZ4 1.9.3 QUIET)
if(NOT LZ4_FOUND)
    set(LZ4_LIB_NAME "lz4")

    add_subdirectory(${LZ4_LIB_NAME}/build/cmake/)
endif()
list(APPEND ALL_SYSTEM_LIBS lz4)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# nlohmann_json
#╒◖═════════════════════════════════════════════════════════════════════◗╕
#find_package(nlohmann_json 3.2.0 QUIET)
if(NOT nlohmann_json_FOUND)
    set(JSON_LIB_NAME "json")

    set(JSON_BuildTests OFF CACHE INTERNAL "")

    add_subdirectory(${JSON_LIB_NAME})
endif()
list(APPEND ALL_SYSTEM_LIBS nlohmann_json)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

#assimp
#╒◖═════════════════════════════════════════════════════════════════════◗╕
find_package(assimp REQUIRED)
if(NOT assimp_FOUND)
    set(ASSIMP_LIB_NAME "assimp")
    set(ASSIMP_INC_PATH ${ASSIMP_LIB_NAME}/include)

    include_directories(${ASSIMP_INC_PATH})
    add_subdirectory(${ASSIMP_LIB_NAME})
endif()
list(APPEND ALL_SYSTEM_LIBS assimp)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

# imgui, imguizmo & iconfontcppheaders
#╒◖═════════════════════════════════════════════════════════════════════◗╕
set(IMGUI_LIB_NAME "imgui")
set(IMGUIZMO_LIB_NAME "imguizmo")
set(ICON_HEADERS_LIB_NAME "iconfontcppheaders")

set(IMGUI_SOURCES
        "${IMGUI_LIB_NAME}/imgui.cpp"
        "${IMGUI_LIB_NAME}/imgui_demo.cpp"
        "${IMGUI_LIB_NAME}/imgui_draw.cpp"
        "${IMGUI_LIB_NAME}/imgui_tables.cpp"
        "${IMGUI_LIB_NAME}/imgui_widgets.cpp"
        #"${IMGUI_LIB_NAME}/backends/imgui_impl_glfw.cpp"
        #"${IMGUI_LIB_NAME}/backends/imgui_impl_opengl3.cpp"
        #"${IMGUI_LIB_NAME}/backends/imgui_impl_vulkan.cpp"

        "${IMGUIZMO_LIB_NAME}/GraphEditor.cpp"
        "${IMGUIZMO_LIB_NAME}/ImCurveEdit.cpp"
        "${IMGUIZMO_LIB_NAME}/ImGradient.cpp"
        "${IMGUIZMO_LIB_NAME}/ImGuizmo.cpp"
        "${IMGUIZMO_LIB_NAME}/ImSequencer.cpp"
        )

set(IMGUI_HEADERS
        "${IMGUI_LIB_NAME}/imconfig.h"
        "${IMGUI_LIB_NAME}/imgui.h"
        "${IMGUI_LIB_NAME}/imgui_internal.h"
        "${IMGUI_LIB_NAME}/imstb_rectpack.h"
        "${IMGUI_LIB_NAME}/imstb_textedit.h"
        "${IMGUI_LIB_NAME}/imstb_truetype.h"
        "${IMGUI_LIB_NAME}/backends/imgui_impl_glfw.h"
        #"${IMGUI_LIB_NAME}/backends/imgui_impl_opengl3.h"
        "${IMGUI_LIB_NAME}/backends/imgui_impl_vulkan.h"

        "${IMGUIZMO_LIB_NAME}/GraphEditor.h"
        "${IMGUIZMO_LIB_NAME}/ImCurveEdit.h"
        "${IMGUIZMO_LIB_NAME}/ImGradient.h"
        "${IMGUIZMO_LIB_NAME}/ImGuizmo.h"
        "${IMGUIZMO_LIB_NAME}/ImSequencer.h"
        "${IMGUIZMO_LIB_NAME}/ImZoomSlider.h"
        )

set(IMGUI_INC_PATH "${IMGUI_LIB_NAME}/")
set(IMGUIZMO_INC_PATH "${IMGUIZMO_LIB_NAME}/")

add_library(${IMGUI_LIB_NAME}
        STATIC
        ${IMGUI_SOURCES}
        ${IMGUI_HEADERS}
        )

target_compile_definitions(${IMGUI_LIB_NAME}
        PRIVATE
        #IMGUI_IMPL_OPENGL_LOADER_GLAD=1
        USE_IMGUI_API
        )

target_include_directories(${IMGUI_LIB_NAME}
        PUBLIC
        "${IMGUI_INC_PATH}"
        "${IMGUIZMO_INC_PATH}"
        "${GLFW_INC_PATH}"
        "${GLAD_INC_PATH}"
        "${ICON_HEADERS_LIB_NAME}"
        )

list(APPEND ALL_SYSTEM_LIBS imgui)
#╘◖═════════════════════════════════════════════════════════════════════◗╛

set(FUSION_ALL_SYSTEM_LIBS ${ALL_SYSTEM_LIBS} CACHE INTERNAL "")
