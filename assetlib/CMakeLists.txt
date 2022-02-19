cmake_minimum_required(VERSION 3.21)
project(assetlib)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_SOURCES src/*.cpp)
file(GLOB_RECURSE SRC_HEADERS src/*.hpp )
add_library(${PROJECT_NAME} STATIC ${SRC_SOURCES} ${SRC_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC src)

target_link_libraries(${PROJECT_NAME} PRIVATE
        nlohmann_json::nlohmann_json
        lz4)