cmake_minimum_required(VERSION 3.8)
project(quill)

#-------------------------------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------------------------------

# Builds Quill without exceptions by adding the -fno-exceptions flag to the compiler.
option(QUILL_NO_EXCEPTIONS "Build without exceptions using -fno-exceptions flag" OFF)

# Enables the use of _mm_prefetch, _mm_clflush, and _mm_clflushopt instructions to enhance cache coherence performance on x86 architectures.
# When enabled, Quill will utilize these instructions on the frontend's queue operations.
# Ensure to specify the target architecture with -march="..." when compiling to maximize compatibility and performance.
option(QUILL_X86ARCH "Enable optimizations for cache coherence on x86 architectures using specific CPU instructions" OFF)

# When enabled, removes the non-prefixed `LOG_*` macros, leaving only `QUILL_LOG_*` macros available.
# This is useful in scenarios where the original macro names conflict with those of an existing logging library.
option(QUILL_DISABLE_NON_PREFIXED_MACROS "Disable non-prefixed macros" ON)

option(QUILL_ENABLE_INSTALL "Enable CMake Install when Quill is not a master project" OFF)

#-------------------------------------------------------------------------------------------------------
# Use newer policies if possible, up to most recent tested version of CMake.
#-------------------------------------------------------------------------------------------------------
cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})

#-------------------------------------------------------------------------------------------------------
# Determine if quill is built as a subproject (using add_subdirectory) or if it is the master project.
#-------------------------------------------------------------------------------------------------------
set(QUILL_MASTER_PROJECT FALSE CACHE BOOL "Master Project" FORCE)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(QUILL_MASTER_PROJECT TRUE CACHE BOOL "Master Project" FORCE)
endif ()

#-------------------------------------------------------------------------------------------------------
# Custom cmake functions
#-------------------------------------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/quill/cmake")
include(Utility)

#-------------------------------------------------------------------------------------------------------
# Resolve version
#-------------------------------------------------------------------------------------------------------
quill_extract_version()

project(quill VERSION ${QUILL_VERSION} LANGUAGES CXX)

#-------------------------------------------------------------------------------------------------------
# Set default build to release
#-------------------------------------------------------------------------------------------------------
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif ()

#---------------------------------------------------------------------------------------
# Compiler config
#---------------------------------------------------------------------------------------
if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif ()

#-------------------------------------------------------------------------------------------------------
# Required Packages
#-------------------------------------------------------------------------------------------------------
find_package(Threads REQUIRED)

if (QUILL_BUILD_TESTS)
    enable_testing()

    if (QUILL_USE_VALGRIND)
        # find valgrind
        find_program(MEMORYCHECK_COMMAND NAMES valgrind)
        if (NOT MEMORYCHECK_COMMAND)
            message(WARNING "Valgrind not found")
        endif ()

        # set valgrind params
        set(MEMORYCHECK_COMMAND_OPTIONS "--tool=memcheck --leak-check=full --leak-resolution=med --show-leak-kinds=all --track-origins=yes --vgdb=no --fair-sched=yes")

        # add memcheck test action to ctest
        include(CTest)
    endif ()
endif ()

#-------------------------------------------------------------------------------------------------------
# Log Info
#-------------------------------------------------------------------------------------------------------
if (QUILL_MASTER_PROJECT)
    option(QUILL_VERBOSE_MAKEFILE "Verbose make output" OFF)

    message(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
    message(STATUS "QUILL_VERSION: ${QUILL_VERSION}")
endif ()

message(STATUS "QUILL_NO_EXCEPTIONS: " ${QUILL_NO_EXCEPTIONS})
message(STATUS "QUILL_NO_THREAD_NAME_SUPPORT: " ${QUILL_NO_THREAD_NAME_SUPPORT})
message(STATUS "QUILL_X86ARCH: " ${QUILL_X86ARCH})
message(STATUS "QUILL_DISABLE_NON_PREFIXED_MACROS: " ${QUILL_DISABLE_NON_PREFIXED_MACROS})


add_subdirectory(quill)
