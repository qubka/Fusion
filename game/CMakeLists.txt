cmake_minimum_required(VERSION 3.21)
project(game)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE engine)

target_include_directories(${PROJECT_NAME} PRIVATE ../engine/src)

