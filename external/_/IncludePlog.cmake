find_package(plog QUIET)
if(NOT plog_FOUND)
    set(PLOG_BUILD_SAMPLES OFF CACHE INTERNAL "")
    add_subdirectory(plog)
endif()
list(APPEND ALL_LIBS plog)