cmake_minimum_required(VERSION 3.21)
project(fusion)

set(CMAKE_CXX_STANDARD 17)

set(SHADER_DIR "${PROJECT_SOURCE_DIR}/data/shaders")
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

