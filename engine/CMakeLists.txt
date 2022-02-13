cmake_minimum_required(VERSION 3.21)
project(engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-msse -msse2 -msse3")

add_subdirectory(external)

file(GLOB_RECURSE COMMON_SRC src/Fusion/*.cpp)
set(HEADER_FILES src/fspch.hpp)

add_library(${PROJECT_NAME} STATIC ${COMMON_SRC} ${PLATFORM_SRC})

target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADER_FILES})

message("${YAML_LIB_INC_PATH}")
target_include_directories(${PROJECT_NAME} PUBLIC external src)
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        glfw
        imgui
        glm
        entt
        magic_enum
        stb
        #glad
        #glm_static    # if build with add_subdirectory
        plog
        yaml-cpp
        portable_file_dialogs
)

# defines
target_compile_definitions(${PROJECT_NAME} PUBLIC FE_DEBUG GLFW_INCLUDE_VULKAN VULKAN_HPP_DISABLE_ENHANCED_MODE)