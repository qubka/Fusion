cmake_minimum_required(VERSION 3.21)
project(fusion)

include(version.cmake)
include(git.cmake)

if(ANDROID)
    set(PLATFORM_FOLDER "android")
else()
    set(PLATFORM_FOLDER "pc")
endif()

file(GLOB_RECURSE SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/fusion/*.cpp" "src/platform/${PLATFORM_FOLDER}/*.cpp")
file(GLOB_RECURSE HDR_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/fusion/*.hpp" "src/platform/${PLATFORM_FOLDER}/*.hpp")
set(PCH_FILE "src/pch.hpp")

# create library and exectuable
add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${HDR_FILES})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${PCH_FILE})
target_include_directories(${PROJECT_NAME} PUBLIC "src")

# copy all asset files to the binary directory of the engine
file(GLOB_RECURSE ASSET_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "assets/*")
foreach(ASSET_FILE ${ASSET_FILES})
    configure_file(${ASSET_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${ASSET_FILE} COPYONLY)
endforeach()

target_link_libraries(${PROJECT_NAME} PUBLIC
        git-info
        ${FUSION_ALL_SYSTEM_LIBS}
        )

if(ANDROID)
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
        FUSION_VERSION_VARIANT=${FUSION_VERSION_VARIANT}
        FUSION_VERSION_MAJOR=${FUSION_VERSION_MAJOR}
        FUSION_VERSION_MINOR=${FUSION_VERSION_MINOR}
        FUSION_VERSION_PATCH=${FUSION_VERSION_PATCH}
        FUSION_COMPILED_COMPILER="${CMAKE_CXX_COMPILER}"
        FUSION_COMPILED_SYSTEM="${CMAKE_SYSTEM}"
        FUSION_COMPILED_GENERATOR="${CMAKE_GENERATOR}"
        )