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
file(GLOB_RECURSE HDR_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/fusion/*.h" "src/platform/${PLATFORM_FOLDER}/*.h")
set(PCH_FILE "src/pch.h")

# create library and exectuable
add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${HDR_FILES})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${PCH_FILE})
target_include_directories(${PROJECT_NAME} PUBLIC "src" ${FUSION_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${FUSION_LIBRARY_DIRS})

# copy all asset files to the binary directory of the engine
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets
        ${CMAKE_CURRENT_BINARY_DIR}/../assets)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/../mono
        ${CMAKE_CURRENT_BINARY_DIR}/../mono)

target_link_libraries(${PROJECT_NAME} PUBLIC
        git-info
        ${FUSION_LINK_LIBS}
        )

if(LINUX)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    link_libraries(${CMAKE_THREAD_LIBS_INIT})
endif()

# defines
target_compile_definitions(${PROJECT_NAME} PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_compile_definitions(${PROJECT_NAME} PUBLIC FUSION_DEBUG)
#target_compile_definitions(${PROJECT_NAME} PUBLIC FUSION_PROFILE)
target_compile_definitions(${PROJECT_NAME} PUBLIC _DEBUG)
target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_USER_CONFIG="fusion/imgui/imconfig.h")
if(NOT ANDROID)
    target_compile_definitions(${PROJECT_NAME} PUBLIC FUSION_SCRIPTING)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC
        FUSION_VERSION_VARIANT=${FUSION_VERSION_VARIANT}
        FUSION_VERSION_MAJOR=${FUSION_VERSION_MAJOR}
        FUSION_VERSION_MINOR=${FUSION_VERSION_MINOR}
        FUSION_VERSION_PATCH=${FUSION_VERSION_PATCH}
        FUSION_COMPILED_COMPILER="${CMAKE_CXX_COMPILER}"
        FUSION_COMPILED_SYSTEM="${CMAKE_SYSTEM}"
        FUSION_COMPILED_GENERATOR="${CMAKE_GENERATOR}"
        )

#### Windows only: Copy the Physx dll files to the simulation executable####

if(WIN32)
    # copy PhysX dll's to build dir. Happens on every build.
    #add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
   #         COMMAND ${CMAKE_COMMAND} -E copy_directory "${PhysxOutputPath}" "$<TARGET_FILE_DIR:${PROJECT_NAME}")
endif()