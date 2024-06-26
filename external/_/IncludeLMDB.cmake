cmake_minimum_required(VERSION 3.1.0)
project(lmdb C)

# Provides ${CMAKE_INSTALL_LIBDIR/BINDIR}
include(GNUInstallDirs)

set(MDB_DEVEL "" CACHE STRING "Enable functions under development")
set(MDB_DEBUG "0" CACHE STRING "Enable debug output.  Needs variable argument macros (a C99 feature).
Set this to 1 for copious tracing. Set to 2 to add dumps of all IDLs
read from and written to the database (used for free space management).")

set(MDB_USE_POSIX_SEM "" CACHE STRING "Use posix semiphores (instead of mutexes) from <semiphore.h>")
set(MDB_DSYNC "" CACHE STRING "Use O_DSYNC on write to avoid extra syscall to flush.")
set(MDB_FDATASYNC "fsync" CACHE STRING "Function to use for syncing, define to `fsync` if `fdatasync` doesn exist")
set(MDB_FDATASYNC_WORKS "1" CACHE STRING "Use if fdatasync is guarenteed to work. May not be the case for old kernels and ext3/4.")
set(MDB_USE_PWRITEV "" CACHE STRING "If the `pwritev` function is available.")
set(MDB_USE_ROBUST "" CACHE STRING "Some platforms define the EOWNERDEAD error code
 even though they don't support Robust Mutexes.
 Unset or use MDB_USE_POSIX_SEM on such platforms.")

set(MDB_ROOT_DIR lmdb/libraries/liblmdb)

add_library(lmdb STATIC ${MDB_ROOT_DIR}/mdb.c ${MDB_ROOT_DIR}/midl.c)
set_property(TARGET lmdb PROPERTY PUBLIC_HEADER MDB_ROOT_DIR/lmdb.h)
set_property(TARGET lmdb PROPERTY PRIVATE_HEADER MDB_ROOT_DIR/midl.h)
set_property(TARGET lmdb PROPERTY C_STANDARD 99)

install(TARGETS lmdb
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
target_include_directories(lmdb PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

export(TARGETS lmdb FILE LmdbConfig.cmake)

list(APPEND ALL_LIBS lmdb)
