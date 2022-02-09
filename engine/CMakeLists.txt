cmake_minimum_required(VERSION 3.21)
project(engine)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external)

set(RENDERER_NAME OpenGL)
find_package(${RENDERER_NAME} REQUIRED)

file(GLOB_RECURSE COMMON_SRC src/Fusion/*.cpp)
file(GLOB_RECURSE PLATFORM_SRC src/Platform/${CMAKE_SYSTEM_NAME}/*.cpp src/Platform/Shared/*.cpp)
file(GLOB_RECURSE RENDERER_SRC src/Renderer/${RENDERER_NAME}/*.cpp)
set(HEADER_FILES src/fspch.hpp)

add_library(${PROJECT_NAME} STATIC ${COMMON_SRC} ${PLATFORM_SRC} ${RENDERER_SRC})

target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADER_FILES})

# OpenGL
if(${RENDERER_NAME} STREQUAL "OpenGL")
    message("Looking for OpenGL dependencies")
    target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::OpenGL glad)
elseif(${RENDERER_NAME} STREQUAL "Vulkan")
    message("Looking for Vulkan dependencies")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC external src)
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        glfw
        imgui
        glm
        entt
        #glm_static    # if build with add_subdirectory
        spdlog::spdlog
        )

# defines
target_compile_definitions(${PROJECT_NAME} PRIVATE FS_DEBUG)