cmake_minimum_required(VERSION 3.21)
project(fusion)

include(version.cmake)
include(git_version.cmake)

if(ANDROID)
    set(PLATFORM_FOLDER "android")
else()
    set(PLATFORM_FOLDER "pc")
endif()

file(GLOB_RECURSE SRC_SOURCES "src/fusion/*.cpp" "src/platform/${PLATFORM_FILES}/*.cpp")
file(GLOB_RECURSE SRC_HEADERS "src/fusion/*.hpp" "src/platform/${PLATFORM_FILES}/*.hpp")
set(HEADER_FILES "src/pch.hpp")

# create library and exectuable
add_library(${PROJECT_NAME} STATIC ${SRC_SOURCES} ${SRC_HEADERS})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC "src")

target_link_libraries(${PROJECT_NAME} PUBLIC
        git-info
        ${FUSION_ALL_SYSTEM_LIBS}
        )

if(ANDROID)
    #target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_HPP_NO_SMART_HANDLE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
    set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
    include_directories(${APP_GLUE_DIR})
    add_library(app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)
endif()

if(NOT WIN32)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    link_libraries(${CMAKE_THREAD_LIBS_INIT})
endif()

# defines
target_compile_definitions(${PROJECT_NAME} PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_compile_definitions(${PROJECT_NAME} PUBLIC FUSION_DEBUG)
#target_compile_definitions(${PROJECT_NAME} PUBLIC FUSION_PROFILE)
target_compile_definitions(${PROJECT_NAME} PUBLIC _DEBUG)
target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_USER_CONFIG="fusion/imgui/imconfig.hpp")

target_compile_definitions(${PROJECT_NAME} PUBLIC
        FUSION_VERSION_MAJOR=${FUSION_VERSION_MAJOR}
        FUSION_VERSION_MINOR=${FUSION_VERSION_MINOR}
        FUSION_VERSION_PATCH=${FUSION_VERSION_PATCH}
        FUSION_COMPILED_COMPILER="${CMAKE_CXX_COMPILER}"
        FUSION_COMPILED_SYSTEM="${CMAKE_SYSTEM}"
        FUSION_COMPILED_GENERATOR="${CMAKE_GENERATOR}"
        )