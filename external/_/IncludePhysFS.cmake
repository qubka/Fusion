find_package(PhysFS QUIET)
if(NOT PhysFS_FOUND)
    if(BUILD_SHARED_LIBS)
        set(PHYSFS_BUILD_STATIC OFF CACHE INTERNAL "Build static library.")
    else()
        set(PHYSFS_BUILD_SHARED OFF CACHE INTERNAL "Build shared library.")
    endif()
    set(PHYSFS_BUILD_TEST OFF CACHE INTERNAL "Build stdio test program.")
    set(PHYSFS_BUILD_DOCS OFF CACHE INTERNAL "Build doxygen based documentation.")
    set(PHYSFS_TARGETNAME_DIST "physfs-dist" CACHE INTERNAL STRING)
    set(PHYSFS_TARGETNAME_UNINSTALL "physfs-uninstall" CACHE INTERNAL STRING)

    add_subdirectory(physfs)
endif()
list(APPEND ALL_LIBS physfs-static)