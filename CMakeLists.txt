cmake_minimum_required(VERSION 3.24)

project(
    Polos
    VERSION 0.1.0
    LANGUAGES CXX C
)

set(CMAKE_CXX_EXTENSIONS OFF)
include(GNUInstallDirs)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(STATIC_STD       "Link static libraries of STL" ON)
option(USE_OPENGL       "Use Polos with OpenGL" ON)
option(USE_VULKAN       "Use Polos with Vulkan" OFF)
option(ENABLE_PROFILING "Enable the profiling tools" OFF)
option(GUI_USE_IMGUI    "Use ImGui for default Gui implementation" ON)
option(BUILD_EDITOR     "If not checked, only builds the runtime binaries" ON)
option(EXPORT_ENGINE    "Will install everything under bin folder." OFF)

set(POLOS_BINARY_DIR  "${CMAKE_CURRENT_BINARY_DIR}")
set(POLOS_DIR         "${CMAKE_CURRENT_LIST_DIR}/Source")
set(POLOS_INCLUDE_DIR "${POLOS_DIR}/Runtime")

if(${CMAKE_BUILD_TYPE} MATCHES "Release" OR ${CMAKE_BUILD_TYPE} MATCHES "MinSizeRel")
    add_compile_definitions(PL_RELEASE)
    message("-- Selected Release mode.")
else()
    add_compile_definitions(PL_DEBUG)
    message("-- Selected Debug mode.")
endif()

configure_file(polos_config.h.in ${POLOS_INCLUDE_DIR}/polos_config.h USE_SOURCE_PERMISSIONS)

set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
if((NOT ${EXPORT_ENGINE}) AND ${BUILD_EDITOR})
    set(INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/Source/Editor")
    message("-- Building editor but not exporting.")
    message("-- Current build dir: ${CMAKE_CURRENT_BINARY_DIR}")
elseif(${EXPORT_ENGINE} AND (NOT ${BUILD_EDITOR}))
    set(INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
    message("-- Not building the editor, exporting engine as a framework.")
    message("-- Current build dir: ${CMAKE_CURRENT_BINARY_DIR}")
elseif(${EXPORT_ENGINE} AND ${BUILD_EDITOR})
    message("-- Building editor and exporting.")
    message("-- Current build dir: ${CMAKE_CURRENT_BINARY_DIR}")
else()
    message(FATAL_ERROR "Building the engine without exporting or without the editor is not allowed.")
    return()
endif()

message("-- STATIC_STD: ${STATIC_STD}")
message("-- USE_OPENGL: ${USE_OPENGL}")
message("-- USE_VULKAN: ${USE_VULKAN}")
message("-- ENABLE_PROFILING: ${ENABLE_PROFILING}")
message("-- GUI_USE_IMGUI: ${GUI_USE_IMGUI}")
message("-- BUILD_EDITOR: ${BUILD_EDITOR}")
message("-- EXPORT_ENGINE: ${EXPORT_ENGINE}")

set(POLOS_INSTALL_INCLUDES_DIR "${INSTALL_DIR}/include/polos")
set(POLOS_INSTALL_LIBS_DIR "${INSTALL_DIR}")

include(CMake/target_build.cmake)

add_compile_options(
    "$<$<C_COMPILER_ID:Clang>:-fcolor-diagnostics>"
    "$<$<C_COMPILER_ID:Clang>:-fansi-escape-codes>"
    "$<$<C_COMPILER_ID:GNU>:-fdiagnostic s-color=always>"
)

add_compile_definitions(
    UUID_SYSTEM_GENERATOR
    UUID_USING_CXX20_SPAN
    $<$<CXX_COMPILER_ID:Clang,GNU>:LIBUUID_CPP20_OR_GREATER>
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
)

if(MSVC)
    string(REPLACE "/Zi" "/ZI" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE "/Zi" "/ZI" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
        
    set(${CMAKE_EXE_LINKER_FLAGS_DEBUG} "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /INCREMENTAL")
    set(${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /INCREMENTAL")
endif()

# ================================== #
#       Build the whole source       #
# ================================== #

add_subdirectory(Source)