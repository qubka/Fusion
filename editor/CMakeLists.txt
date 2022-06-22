cmake_minimum_required(VERSION 3.21)
project(editor)

# Force all compilers to use the correct C and C++ standard versions.
# Needed to allow for CheckCXXSourceCompiles to work correctly.
set(CMAKE_REQUIRED_FLAGS "-std=c++17 -std=c11")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
#find_package(SQLite3 REQUIRED)

file(GLOB_RECURSE SRC_SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} "src/main.cpp" ${SRC_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        fusion
        #sqlite3
        portable_file_dialogs
        )

target_include_directories(${PROJECT_NAME} PRIVATE src)