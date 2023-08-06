find_package(bitmask QUIET)
if(NOT bitmask_FOUND)
    add_library(bitmask INTERFACE)

    target_include_directories(bitmask INTERFACE bitmask/include)
endif()
list(APPEND ALL_LIBS bitmask)