cmake_minimum_required(VERSION 3.21)
project(editor)

file(GLOB_RECURSE SRC_SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} "src/main.cpp" ${SRC_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE fusion)

target_include_directories(${PROJECT_NAME} PRIVATE "src")