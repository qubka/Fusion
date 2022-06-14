# GLFW
set(GLFW_LIB_NAME "glfw")
set(GLFW_INC_PATH ${GLFW_LIB_NAME}/include)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#set(GLFW_VULKAN_STATIC  OFF CACHE BOOL "" FORCE)    # OFF by default
#set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)    # OFF by default
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

add_subdirectory(${GLFW_LIB_NAME})


# plog
set(PLOG_LIB_NAME "plog")

set(PLOG_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${PLOG_LIB_NAME})


# spdlog
#set(SPDLOG_LIB_NAME "spdlog")

#set(SPDLOG_MASTER_PROJECT OFF CACHE BOOL "" FORCE)

#add_subdirectory(${SPDLOG_LIB_NAME})

# OR
#set(SPDLOG_LIB_NAME "spdlog")
#set(SPDLOG_SRC_PATH ${SPDLOG_LIB_NAME}/src)
#set(SPDLOG_INC_PATH ${SPDLOG_LIB_NAME}/include)

#add_library(${SPDLOG_LIB_NAME}
#    STATIC
#        ${SPDLOG_SRC_PATH}/spdlog.cpp
#)

#target_include_directories(${SPDLOG_LIB_NAME}
#    PUBLIC
#        ${SPDLOG_INC_PATH})


# glm
set(GLM_LIB_NAME "glm")
set(GLM_INC_PATH ${GLM_LIB_NAME})

add_library(${GLM_LIB_NAME} INTERFACE)

target_include_directories(${GLM_LIB_NAME}
        INTERFACE
        ${GLM_INC_PATH}
        )

# OR
#set(GLM_LIB_NAME "glm")
#set(GLM_INC_PATH ${GLM_LIB_NAME}/glm)

#set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)

#add_subdirectory(${GLM_LIB_NAME})

#target_include_directories(${GLM_LIB_NAME}
#    PUBLIC
#        ${GLM_INC_PATH}
#)


# gli
set(GLI_LIB_NAME "gli")
set(GLI_INC_PATH ${GLI_LIB_NAME})

add_library(${GLI_LIB_NAME} INTERFACE)

target_include_directories(${GLI_LIB_NAME}
        INTERFACE
        ${GLI_INC_PATH}
        )


# glad
#set(GLAD_LIB_NAME "glad")
#set(GLAD_SRC_PATH "${GLAD_LIB_NAME}/src")
#set(GLAD_INC_PATH "${GLAD_LIB_NAME}/include")
#
#add_library( ${GLAD_LIB_NAME}
#        STATIC
#        "${GLAD_SRC_PATH}/glad.c"
#        )
#
#target_include_directories(${GLAD_LIB_NAME}
#        PUBLIC
#        "${GLAD_INC_PATH}"
#        )


# entt
set(ENTT_LIB_NAME "entt")
set(ENTT_INC_PATH ${ENTT_LIB_NAME}/src)

add_library(${ENTT_LIB_NAME} INTERFACE)

target_include_directories(${ENTT_LIB_NAME}
        INTERFACE
        ${ENTT_INC_PATH}
        )


# magic_enum
set(MAGIC_ENUM_LIB_NAME "magic_enum")
set(MAGIC_ENUM_INC_PATH ${MAGIC_ENUM_LIB_NAME}/include)

add_library(${MAGIC_ENUM_LIB_NAME} INTERFACE)

target_include_directories(${MAGIC_ENUM_LIB_NAME}
        INTERFACE
        ${MAGIC_ENUM_INC_PATH}
        )

# OR
#set(ENTT_LIB_NAME "ENTT")
#set(ENTT_INC_PATH ${ENTT_LIB_NAME}/ENTT)

#set(ENTT_TEST_ENABLE OFF CACHE BOOL "" FORCE)

#add_subdirectory(${ENTT_LIB_NAME})

#target_include_directories(${ENTT_LIB_NAME}
#    PUBLIC
#        ${ENTT_INC_PATH}
#)


# stb
set(STB_LIB_NAME "stb")
set(STB_INC_PATH ${STB_LIB_NAME}/)

add_library(${STB_LIB_NAME} INTERFACE)

target_include_directories(${STB_LIB_NAME}
        INTERFACE
        ${STB_INC_PATH}
        )


# imgui, imguizmo & iconfontcppheaders
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

# yaml-cpp
set(YAML_LIB_NAME "yaml-cpp")

add_subdirectory(${YAML_LIB_NAME})

set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_BUILD_SHARED_LIBS ON)


# portfiledialog
set(PFD_LIB_NAME "portable-file-dialogs")

add_subdirectory(${PFD_LIB_NAME})


# lz4
set(LZ4_LIB_NAME "lz4")

add_subdirectory(${LZ4_LIB_NAME}/build/cmake/)


# freetype
set(FREETYPE_LIB_NAME "freetype")

add_subdirectory(${FREETYPE_LIB_NAME})


# nlohmann_json
set(JSON_LIB_NAME "json")

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(${JSON_LIB_NAME})

# physx
# PHYSX_ROOT_DIR - path to the `{cloned repository}/physx` repo directory git://github.com/NVIDIAGameWorks/PhysX.git
set(PHYSX_ROOT_DIR ${CMAKE_SOURCE_DIR}/external/physx/physx) #This is needed for ${PHYSX_ROOT_DIR}/compiler/public/CMakeLists.txt
set(PHYSX_INCLUDE_DIRS ${PHYSX_ROOT_DIR}/include/ $ENV{PHYSX_ROOT_DIR}/../pxshared/include/)
#set(PHYSX_LIBRARIES
#       PhysXExtensions
#       PhysX
#       PhysXPvdSDK
#       PhysXVehicle
#       PhysXCharacterKinematic
#       PhysXCooking
#       PhysXCommon
#       PhysXFoundation
#       # SnippetUtils
#       )

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