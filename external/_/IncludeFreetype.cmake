find_package(Freetype QUIET)
if(NOT Freetype_FOUND)
    foreach(_freetype_option "FT_DISABLE_ZLIB" "FT_DISABLE_BZIP2" "FT_DISABLE_PNG" "FT_DISABLE_HARFBUZZ" "FT_DISABLE_BROTLI")
        set(${_freetype_option} TRUE CACHE INTERNAL "")
    endforeach()

	add_subdirectory(freetype)
endif()
list(APPEND ALL_LIBS freetype)