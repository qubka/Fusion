find_package(cereal QUIET)
if(NOT cereal_FOUND)
    set(BUILD_DOC OFF CACHE INTERNAL "")
    set(BUILD_SANDBOX OFF CACHE INTERNAL "")
    set(SKIP_PERFORMANCE_COMPARISON ON CACHE INTERNAL "")

    add_subdirectory(cereal)
endif()
list(APPEND ALL_LIBS cereal)