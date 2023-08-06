find_package(glslang QUIET)
if(NOT glslang_FOUND)
    set(GLSLANG_LIB_NAME "glslang")

    # On MSVC shared mode must be disabled with glslang currently
    set(BUILD_SHARED_LIBS_SAVED "${BUILD_SHARED_LIBS}")
    if(MSVC)
        set(BUILD_SHARED_LIBS OFF)
    endif()
    set(ENABLE_SPVREMAPPER OFF CACHE INTERNAL "Enables building of SPVRemapper")
    set(ENABLE_GLSLANG_BINARIES OFF CACHE INTERNAL "Builds glslangValidator and spirv-remap")
    set(ENABLE_HLSL OFF CACHE INTERNAL "Enables HLSL input support")
    set(ENABLE_RTTI ON CACHE INTERNAL "Enables RTTI")
    set(ENABLE_EXCEPTIONS ON CACHE INTERNAL "Enables Exceptions")
    set(ENABLE_CTEST OFF CACHE INTERNAL "Enables testing")
    set(SKIP_GLSLANG_INSTALL ON CACHE INTERNAL "Skip installation")

    add_subdirectory(${GLSLANG_LIB_NAME})

    # Reset back to value before MSVC fix
    set(BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS_SAVED}")

    set_target_properties(glslang PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
    #set_target_properties(glslang-build-info PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
    set_target_properties(GenericCodeGen PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
    set_target_properties(MachineIndependent PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
    set_target_properties(OGLCompiler PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
    set_target_properties(OSDependent PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
    set_target_properties(SPIRV PROPERTIES FOLDER ${GLSLANG_LIB_NAME})
endif()
list(APPEND ALL_LIBS glslang SPIRV)