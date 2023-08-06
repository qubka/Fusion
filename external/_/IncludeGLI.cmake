find_package(gli QUIET)
if(NOT gli_FOUND)
    add_library(gli INTERFACE)

    target_include_directories(gli INTERFACE gli/)
endif()
list(APPEND ALL_LIBS gli)