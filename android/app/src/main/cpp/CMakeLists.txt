cmake_minimum_required(VERSION 3.21)
project(fusionjni)

# Include the GameActivity static lib to the project.
find_package(game-activity REQUIRED CONFIG)
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u \
    Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../external ${CMAKE_CURRENT_BINARY_DIR}/external)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../engine ${CMAKE_CURRENT_BINARY_DIR}/engine)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE "src")

# add lib dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE fusion)
