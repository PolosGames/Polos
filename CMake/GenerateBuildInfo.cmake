# Run with -P. Bumps the per-build-dir counter and regenerates one TU.
# Inputs: BUILD_NUMBER_FILE, TEMPLATE_FILE, OUTPUT_FILE, MODULE_LABEL, SOURCE_DIR

if (EXISTS "${BUILD_NUMBER_FILE}")
    file(READ "${BUILD_NUMBER_FILE}" POLOS_BUILD_NUMBER)
    string(STRIP "${POLOS_BUILD_NUMBER}" POLOS_BUILD_NUMBER)
    math(EXPR POLOS_BUILD_NUMBER "${POLOS_BUILD_NUMBER} + 1")
else()
    set(POLOS_BUILD_NUMBER 1)
endif()
file(WRITE "${BUILD_NUMBER_FILE}" "${POLOS_BUILD_NUMBER}")

find_package(Git QUIET)

# a shallow clone or an archive export has no history - fall back rather than fail the build
set(POLOS_COMMIT_COUNT 0)
set(POLOS_COMMIT "unknown")

if (GIT_FOUND)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-list --count HEAD
        WORKING_DIRECTORY "${SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_COUNT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE GIT_COUNT_RESULT
    )
    if (GIT_COUNT_RESULT EQUAL 0)
        set(POLOS_COMMIT_COUNT "${GIT_COUNT}")
    endif()

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
        WORKING_DIRECTORY "${SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_SHA
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE GIT_SHA_RESULT
    )
    if (GIT_SHA_RESULT EQUAL 0)
        set(POLOS_COMMIT "${GIT_SHA}")

        execute_process(
            COMMAND "${GIT_EXECUTABLE}" status --porcelain --untracked-files=no
            WORKING_DIRECTORY "${SOURCE_DIR}"
            OUTPUT_VARIABLE GIT_DIRTY
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if (NOT GIT_DIRTY STREQUAL "")
            set(POLOS_COMMIT "${POLOS_COMMIT}-dirty")
        endif()
    endif()
endif()

string(TIMESTAMP POLOS_BUILD_TIMESTAMP "%Y-%m-%d %H:%M" UTC)
set(POLOS_MODULE_LABEL "${MODULE_LABEL}")

configure_file("${TEMPLATE_FILE}" "${OUTPUT_FILE}" @ONLY)
