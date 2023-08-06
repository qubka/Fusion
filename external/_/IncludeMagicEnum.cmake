find_package(magic_enum QUIET)
if(NOT magic_enum_FOUND)
    add_library(magic_enum INTERFACE)

    target_include_directories(magic_enum INTERFACE magic_enum/include)
endif()
list(APPEND ALL_LIBS magic_enum)