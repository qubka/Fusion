find_package(LZ4 QUIET)
if(NOT LZ4_FOUND)
    add_subdirectory(lz4/build/cmake/)
endif()
list(APPEND ALL_LIBS lz4)