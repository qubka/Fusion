find_package(GSL QUIET)
if(NOT GSL_FOUND)
    set(GSL_TEST OFF CACHE INTERNAL "")
    add_library(gsl INTERFACE)

    target_include_directories(gsl INTERFACE gsl/include)
endif()
list(APPEND ALL_LIBS gsl)