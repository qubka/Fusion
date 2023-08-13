cmake_minimum_required(VERSION 3.21)
project(fusion)

# Force all compilers to use the correct C and C++ standard versions.
# Needed to allow for CheckCXXSourceCompiles to work correctly.
set(CMAKE_REQUIRED_FLAGS "-std=c++17 -std=c11")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Speed up building on MinGW
if(WIN32)
    add_link_options(-fuse-ld=lld)
    #add_compile_options(-femulated-tls)
endif()

if(NOT "${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    message(FATAL_ERROR "Only 64 bit builds supported.")
endif()

add_subdirectory(external)
add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(editor)

