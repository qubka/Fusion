find_package(glm QUIET)
if(NOT glm_FOUND)
    add_library(glm INTERFACE)

    target_include_directories(glm INTERFACE glm/)
    target_compile_definitions(glm INTERFACE GLM_FORCE_DEPTH_ZERO_TO_ONE)
endif()
list(APPEND ALL_LIBS glm)