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
set(POLOS_INSTALL_DIR  ${CMAKE_INSTALL_PREFIX})
set(POLOS_INSTALL_INC_DIR "${POLOS_INSTALL_DIR}/include/polos")
set(POLOS_INSTALL_LIB_DIR "${POLOS_INSTALL_DIR}")

configure_file(polos_config.h.in ${POLOS_DIR}/polos_config.h USE_SOURCE_PERMISSIONS)

option(ENABLE_PROFILING "Enable the profiling tools" OFF)
option(GUI_USE_IMGUI    "Use ImGui for default Gui implementation" ON)
option(BUILD_EDITOR     "If not checked, only builds the runtime binaries" ON)
option(EXPORT_ENGINE    "Will install everything under bin folder." OFF)

message(STATUS "[POLOS] ENABLE_PROFILING: ${ENABLE_PROFILING}")
message(STATUS "[POLOS] BUILD_EDITOR: ${BUILD_EDITOR}")
message(STATUS "[POLOS] EXPORT_ENGINE: ${EXPORT_ENGINE}")
message(STATUS "[POLOS] POLOS_INSTALL_DIR: ${POLOS_INSTALL_DIR}")

if((NOT ${EXPORT_ENGINE}) AND ${BUILD_EDITOR})
    set(POLOS_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/Source/Editor")
    message(STATUS "[POLOS] Building editor but not exporting.")
    message(STATUS "[POLOS] Current build dir: ${CMAKE_CURRENT_BINARY_DIR}")
elseif(${EXPORT_ENGINE} AND (NOT ${BUILD_EDITOR}))
    set(POLOS_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
    message(STATUS "[POLOS] Not building the editor, exporting engine as a framework.")
    message(STATUS "[POLOS] Current build dir: ${CMAKE_CURRENT_BINARY_DIR}")
elseif(${EXPORT_ENGINE} AND ${BUILD_EDITOR})
    message(STATUS "[POLOS] Building editor and exporting.")
    message(STATUS "[POLOS] Current build dir: ${CMAKE_CURRENT_BINARY_DIR}")
else()
    message(FATAL_ERROR "Building the engine without exporting or without the editor is not allowed.")
    return()
endif()

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
