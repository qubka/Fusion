cmake_minimum_required(VERSION 3.21)
project(engine)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-msse -msse2 -msse3")

if (NOT "${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    message( FATAL_ERROR "Only 64 bit builds supported." )
endif()

find_package(Vulkan REQUIRED)
find_package(assimp REQUIRED) # https://ittone.ma/ittone/c-how-do-i-add-assimp-as-a-dependency-using-cmake-and-clion/

file(GLOB_RECURSE SRC_SOURCES src/fusion/*.cpp)
file(GLOB_RECURSE SRC_HEADERS src/Fusion/*.hpp)
set(HEADER_FILES src/fspch.hpp)

add_library(${PROJECT_NAME} STATIC ${SRC_SOURCES} ${SRC_HEADERS})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC src ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC
    glfw
    imgui
    glm
    gli
    entt
    magic_enum
    stb
    plog
    yaml-cpp
    portable_file_dialogs
    #assetlib
    Vulkan::Vulkan
    assimp::assimp
)

if (ANDROID)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_HPP_NO_SMART_HANDLE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
    set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
    include_directories(${APP_GLUE_DIR})
    add_library(app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)
endif()

if (ANDROID)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_ANDROID_KHR)
elseif (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_WIN32_KHR)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_XCB_KHR)
    #find_package(XCB REQUIRED)
    #link_libraries(${XCB_LIBRARIES})
endif()

if (NOT WIN32)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    link_libraries(${CMAKE_THREAD_LIBS_INIT})
endif()

# defines
target_compile_definitions(${PROJECT_NAME} PUBLIC FE_DEBUG)
target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_INCLUDE_VULKAN)
target_compile_definitions(${PROJECT_NAME} PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)

add_dependencies(${PROJECT_NAME} Shaders)