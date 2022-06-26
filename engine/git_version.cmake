find_package(Git QUIET)

if(GIT_SCM)
    # the commit's SHA1, and whether the building workspace was dirty or not
    # describe --match=NeVeRmAtCh --always --tags --abbrev=40 --dirty
    execute_process(COMMAND
            "${GIT_EXECUTABLE}" --no-pager describe --tags --always --dirty
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE GIT_SHA1
            ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    # branch
    execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # the date of the commit
    execute_process(COMMAND
            "${GIT_EXECUTABLE}" log -1 --format=%ad --date=local
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE GIT_DATE
            ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(COMMAND
            "${GIT_EXECUTABLE}" describe --tags --abbrev=0
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE GIT_TAG
            ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

    # the subject of the commit
    execute_process(COMMAND
            "${GIT_EXECUTABLE}" log -1 --format=%s
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE GIT_COMMIT_SUBJECT
            ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    # remove # from subject
    string(REGEX REPLACE "[\#\"]+"
            "" GIT_COMMIT_SUBJECT
            ${GIT_COMMIT_SUBJECT})
else()
    message(STATUS "Git not installed")
    set(GIT_SHA1 "UNKNOWN")
    set(GIT_DATE "UNKNOWN")
    set(GIT_COMMIT_SUBJECT "UNKNOWN")
    set(GIT_BRANCH "UNKNOWN")
    set(GIT_TAG "UNKNOWN")
endif()

add_library(git-info INTERFACE)
target_compile_definitions(git-info INTERFACE
        GIT_COMMIT_HASH="${GIT_SHA1}"
        GIT_COMMIT_DATE="${GIT_DATE}"
        GIT_TAG="${GIT_TAG}"
        GIT_COMMIT_SUBJECT="${GIT_COMMIT_SUBJECT}"
        GIT_BRANCH="${GIT_BRANCH}"
        )