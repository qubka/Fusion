cmake_minimum_required(VERSION 3.21)
project(editor)

file(GLOB_RECURSE SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE fusion)

target_include_directories(${PROJECT_NAME} PRIVATE "src")