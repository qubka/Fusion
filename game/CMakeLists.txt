cmake_minimum_required(VERSION 3.21)
project(game)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp)

message("$SOLUTION_DIR")

target_link_libraries(${PROJECT_NAME} engine)

target_include_directories(${PROJECT_NAME} PUBLIC ../engine/src)
