cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(Polos
        LANGUAGES CXX C
        DESCRIPTION "Game engine for PolosGames written in C++23")

set(CMAKE_CXX_EXTENSIONS OFF)
include(GNUInstallDirs)

set(POLOS_CXX_STANDARD 23)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(POLOS_BINARY_DIR  "${CMAKE_CURRENT_BINARY_DIR}")
set(POLOS_DIR         "${CMAKE_CURRENT_LIST_DIR}/Source")
set(POLOS_INC_DIR     "${POLOS_DIR}/Runtime")
set(POLOS_EXAMPLES    "${CMAKE_CURRENT_LIST_DIR}/Examples")
set(POLOS_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")

configure_file(polos_config.h.in ${POLOS_DIR}/polos_config.h USE_SOURCE_PERMISSIONS)

option(ENABLE_PROFILING "Enable the profiling tools" OFF)
option(GUI_USE_IMGUI    "Use ImGui for default Gui implementation" ON)
option(BUILD_EDITOR     "If not checked, only builds the runtime binaries" ON)
option(BUILD_EXAMPLES   "Will build all example projects under Examples directory." ON)

set(POLOS_INSTALL_INC_DIR "${POLOS_INSTALL_DIR}/include/polos")
set(POLOS_INSTALL_LIB_DIR "${POLOS_INSTALL_DIR}")

message(STATUS "[POLOS] ENABLE_PROFILING: ${ENABLE_PROFILING}")
message(STATUS "[POLOS] BUILD_EDITOR: ${BUILD_EDITOR}")
message(STATUS "[POLOS] POLOS_INSTALL_DIR: ${POLOS_INSTALL_DIR}")

include(CMake/target_build.cmake)

add_compile_options(
        "$<$<C_COMPILER_ID:Clang>:-fcolor-diagnostics>"
        "$<$<C_COMPILER_ID:Clang>:-fansi-escape-codes>"
        "$<$<C_COMPILER_ID:GNU>:-fdiagnostics-color=always>"
)

add_compile_definitions(
        UUID_SYSTEM_GENERATOR
        UUID_USING_CXX20_SPAN
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
)

if(LINUX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

add_subdirectory(Source)

if (BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif ()
