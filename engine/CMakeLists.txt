cmake_minimum_required(VERSION 3.21)
project(engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-msse -msse2 -msse3")

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SRC_SOURCES src/Fusion/*.cpp)
#file(GLOB_RECURSE SRC_HEADERS src/Fusion/*.hpp)
set(HEADER_FILES src/fspch.hpp)

add_library(${PROJECT_NAME} STATIC ${SRC_SOURCES} ${SRC_HEADERS})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC src ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        glfw
        imgui
        glm
        entt
        magic_enum
        stb
        plog
        yaml-cpp
        portable_file_dialogs
        #assetlib
        Vulkan::Vulkan
)

# defines
target_compile_definitions(${PROJECT_NAME} PUBLIC FE_DEBUG)
target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_INCLUDE_VULKAN)
target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_HPP_DISABLE_ENHANCED_MODE)
target_compile_definitions(${PROJECT_NAME} PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_LEFT_HANDED GLM_FORCE_XYZW_ONLY GLM_FORCE_RADIANS GLM_ENABLE_EXPERIMENTAL)