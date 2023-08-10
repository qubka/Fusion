find_package(assimp QUIET)
if(NOT assimp_FOUND)
    foreach(_assimp_option "ASSIMP_BUILD_ASSIMP_TOOLS" "ASSIMP_BUILD_SAMPLES" "ASSIMP_BUILD_TESTS" "ASSIMP_BUILD_DOCS" "ASSIMP_BUILD_ZLIB" "ASSIMP_INSTALL_PDB")
        set(${_assimp_option} FALSE CACHE INTERNAL "")
    endforeach()
	
    include_directories(assimp/include)
    add_subdirectory(assimp)
endif()
list(APPEND ALL_LIBS assimp)