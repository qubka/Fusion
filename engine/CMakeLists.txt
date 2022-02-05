cmake_minimum_required(VERSION 3.21)
project(engine)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_SOURCES src/*.cpp)
set(HEADER_FILES src/pch.hpp)

add_library(${PROJECT_NAME} SHARED ${SRC_SOURCES})

target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADER_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE ADD_EXPORTS)