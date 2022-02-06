cmake_minimum_required(VERSION 3.21)
project(engine)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external/spdlog)
add_subdirectory(external/glm)
add_subdirectory(external/glfw)

file(GLOB_RECURSE COMMON_SRC src/Fusion/*.cpp)
file(GLOB_RECURSE PLATFORM_SRC src/Platform/${CMAKE_SYSTEM_NAME}/*.cpp)
set(HEADER_FILES src/fspch.hpp)

add_library(${PROJECT_NAME} STATIC ${COMMON_SRC} ${PLATFORM_SRC})

target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC external/glm)

target_include_directories(${PROJECT_NAME} PUBLIC external/glfw/include)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

target_include_directories(${PROJECT_NAME} PUBLIC external/spdlog/include)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog_header_only)

target_compile_definitions(${PROJECT_NAME} PRIVATE FS_ADD_EXPORTS FS_ENABLE_ASSERTS)