cmake_minimum_required(VERSION 3.21)
project(editor)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} editor_app.cpp ${SRC_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE engine)

target_include_directories(${PROJECT_NAME} PRIVATE src)