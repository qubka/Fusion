cmake_minimum_required(VERSION 3.21)
project(editor)

set(CMAKE_CXX_STANDARD 17)

#find_package(SQLite3 REQUIRED)

file(GLOB_RECURSE SRC_SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} src/main.cpp ${SRC_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        engine
        #sqlite3
        portable_file_dialogs
        )

target_include_directories(${PROJECT_NAME} PRIVATE src)