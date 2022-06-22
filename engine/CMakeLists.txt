cmake_minimum_required(VERSION 3.21)
project(fusion)
include(${CMAKE_CURRENT_SOURCE_DIR}/Version.cmake)

# Force all compilers to use the correct C and C++ standard versions.
# Needed to allow for CheckCXXSourceCompiles to work correctly.
set(CMAKE_REQUIRED_FLAGS "-std=c++17 -std=c11")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if (NOT "${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    message(FATAL_ERROR "Only 64 bit builds supported.")
endif()

# the commit's SHA1, and whether the building workspace was dirty or not
execute_process(COMMAND
        "${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --always --abbrev=40 --dirty
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_SHA1
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# the date of the commit
execute_process(COMMAND
        "${GIT_EXECUTABLE}" log -1 --format=%ad --date=local
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_DATE
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# the subject of the commit
execute_process(COMMAND
        "${GIT_EXECUTABLE}" log -1 --format=%s
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_COMMIT_SUBJECT
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

file(GLOB_RECURSE SRC_SOURCES "src/fusion/*.cpp")
file(GLOB_RECURSE SRC_HEADERS "src/fusion/*.hpp")
set(HEADER_FILES "src/pch.hpp")

# generate config.cpp
configure_file("src/config.cpp.in" "src/config.cpp" @ONLY)

list(APPEND SRC_SOURCES "src/config.cpp")
list(APPEND HEADER_FILES "src/config.hpp")

add_library(${PROJECT_NAME} STATIC ${SRC_SOURCES} ${SRC_HEADERS})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC "src")

find_package(assimp REQUIRED) # https://ittone.ma/ittone/c-how-do-i-add-assimp-as-a-dependency-using-cmake-and-clion/

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

target_link_libraries(${PROJECT_NAME} PUBLIC
        volk
        glfw
        imgui
        glm
        gli
        stb
        entt
        magic_enum
        plog
        freetype
        glslang
        SPIRV
        yaml-cpp
        assimp::assimp
        ${PHYSX_LIBRARIES}
        )



if (ANDROID)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_HPP_NO_SMART_HANDLE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
    set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
    include_directories(${APP_GLUE_DIR})
    add_library(app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)
endif()

if (ANDROID)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_ANDROID_KHR)
elseif (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_WIN32_KHR)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_XCB_KHR)
    #find_package(XCB REQUIRED)
    #link_libraries(${XCB_LIBRARIES})
endif()

if (NOT WIN32)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    link_libraries(${CMAKE_THREAD_LIBS_INIT})
endif()

# defines
target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_INCLUDE_VULKAN)
target_compile_definitions(${PROJECT_NAME} PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_compile_definitions(${PROJECT_NAME} PUBLIC FUSION_DEBUG)

add_dependencies(${PROJECT_NAME} Shaders)