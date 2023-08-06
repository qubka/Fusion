find_package(Stb QUIET)
if(NOT Stb_FOUND)
    add_library(stb INTERFACE)

    target_include_directories(stb INTERFACE stb/)
endif()
list(APPEND ALL_LIBS stb)