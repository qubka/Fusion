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

if(NOT "${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    message(FATAL_ERROR "Only 64 bit builds supported.")
endif()

set(SHADER_DIR "${PROJECT_SOURCE_DIR}/content/shaders")
file(GLOB_RECURSE GLSL_SOURCE_FILES
        ${SHADER_DIR}/*.vert
        ${SHADER_DIR}/*.frag
        ${SHADER_DIR}/*.comp
        ${SHADER_DIR}/*.tesc
        ${SHADER_DIR}/*.tese
        ${SHADER_DIR}/*.geom
        )

if (WIN32)
    if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
    else()
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin32/glslangValidator.exe")
    endif()
else()
    set(GLSL_VALIDATOR "/usr/bin/glslc")
endif()

foreach(GLSL ${GLSL_SOURCE_FILES})
    message(STATUS "BUILDING SHADER")
    set(SPIRV "${GLSL}.spv")
    message(STATUS ${GLSL} -> ${SPIRV})
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
        SOURCES ${GLSL_SOURCE_FILES}
)

add_subdirectory(external)
add_subdirectory(engine)
#add_subdirectory(game)
add_subdirectory(editor)

