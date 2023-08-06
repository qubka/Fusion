if(WIN32)
    find_package(GSL QUIET)
    if(NOT GSL_FOUND)
        set(GSL_TEST OFF CACHE INTERNAL "")
        add_library(gsl INTERFACE)

        target_include_directories(gsl INTERFACE gsl/include)
    endif()
    list(APPEND ALL_LIBS gsl)
else() # May be remove ?
    find_package(span QUIET)
    if(NOT span_FOUND)
        add_library(span INTERFACE)

        target_include_directories(span INTERFACE span/include)
    endif()
    list(APPEND ALL_LIBS span)
endif()