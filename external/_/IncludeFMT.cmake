find_package(fmt QUIET)
if(NOT fmt_FOUND)
    add_subdirectory(fmt)
endif()
list(APPEND ALL_LIBS fmt)