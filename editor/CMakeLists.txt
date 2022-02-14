cmake_minimum_required(VERSION 3.21)
project(editor)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE COMMON_SRC src/*.cpp)
add_executable(${PROJECT_NAME} EditorApp.cpp ${COMMON_SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE engine Vulkan::Vulkan)

target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS} src)