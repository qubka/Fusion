# GLFW
set(GLFW_LIB_NAME "glfw")
set(GLFW_INC_PATH ${GLFW_LIB_NAME}/include)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#set(GLFW_VULKAN_STATIC  OFF CACHE BOOL "" FORCE)    # OFF by default
#set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)    # OFF by default
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

add_subdirectory(${GLFW_LIB_NAME})


# spdlog
set(SPDLOG_LIB_NAME "spdlog")

set(SPDLOG_MASTER_PROJECT OFF CACHE BOOL "" FORCE)

add_subdirectory(${SPDLOG_LIB_NAME})

# OR
#set(SPDLOG_LIB_NAME "spdlog")
#set(SPDLOG_SRC_PATH ${SPDLOG_LIB_NAME}/src)
#set(SPDLOG_INC_PATH ${SPDLOG_LIB_NAME}/include)

#add_library(${SPDLOG_LIB_NAME}
#    STATIC
#        ${SPDLOG_SRC_PATH}/spdlog.cpp
#)

#target_include_directories(${SPDLOG_LIB_NAME}
#    PUBLIC
#        ${SPDLOG_INC_PATH})


# glm
set(GLM_LIB_NAME "glm")
set(GLM_INC_PATH ${GLM_LIB_NAME}/glm)

add_library(${GLM_LIB_NAME} INTERFACE)

target_include_directories(${GLM_LIB_NAME}
        INTERFACE
        ${GLM_INC_PATH}
        )

# OR
#set(GLM_LIB_NAME "glm")
#set(GLM_INC_PATH ${GLM_LIB_NAME}/glm)

#set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)

#add_subdirectory(${GLM_LIB_NAME})

#target_include_directories(${GLM_LIB_NAME}
#    PUBLIC
#        $(GLM_INC_PATH)
#)


# glad
set(GLAD_LIB_NAME "glad")
set(GLAD_SRC_PATH "${GLAD_LIB_NAME}/src")
set(GLAD_INC_PATH "${GLAD_LIB_NAME}/include")

add_library( ${GLAD_LIB_NAME}
        STATIC
        "${GLAD_SRC_PATH}/glad.c"
        )

target_include_directories(${GLAD_LIB_NAME}
        PUBLIC
        "${GLAD_INC_PATH}"
        )


# imgui
set(IMGUI_LIB_NAME "imgui")

set(IMGUI_SOURCES
        "${IMGUI_LIB_NAME}/imgui.cpp"
        "${IMGUI_LIB_NAME}/imgui_demo.cpp"
        "${IMGUI_LIB_NAME}/imgui_draw.cpp"
        "${IMGUI_LIB_NAME}/imgui_tables.cpp"
        "${IMGUI_LIB_NAME}/imgui_widgets.cpp"
        "${IMGUI_LIB_NAME}/backends/imgui_impl_glfw.cpp"
        "${IMGUI_LIB_NAME}/backends/imgui_impl_opengl3.cpp"
        )

set(IMGUI_HEADERS
        "${IMGUI_LIB_NAME}/imconfig.h"
        "${IMGUI_LIB_NAME}/imgui.h"
        "${IMGUI_LIB_NAME}/imgui_internal.h"
        "${IMGUI_LIB_NAME}/imstb_rectpack.h"
        "${IMGUI_LIB_NAME}/imstb_textedit.h"
        "${IMGUI_LIB_NAME}/imstb_truetype.h"
        "${IMGUI_LIB_NAME}/backends/imgui_impl_glfw.h"
        "${IMGUI_LIB_NAME}/backends/imgui_impl_opengl3.h"
        )

set(IMGUI_INC_PATH "${IMGUI_LIB_NAME}/")

add_library(${IMGUI_LIB_NAME}
        STATIC
        ${IMGUI_SOURCES}
        ${IMGUI_HEADERS}
        )

target_compile_definitions(${IMGUI_LIB_NAME}
        PRIVATE
        IMGUI_IMPL_OPENGL_LOADER_GLAD=1
        )

target_include_directories(${IMGUI_LIB_NAME}
        PUBLIC
        "${IMGUI_INC_PATH}"
        "${GLFW_INC_PATH}"
        "${GLAD_INC_PATH}"
        )