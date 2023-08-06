find_package(entt QUIET)
if(NOT entt_FOUND)
    add_library(entt INTERFACE)

    target_include_directories(entt INTERFACE entt/src)
endif()
list(APPEND ALL_LIBS entt)