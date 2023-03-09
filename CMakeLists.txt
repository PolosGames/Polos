cmake_minimum_required(VERSION 3.24)

project(
    Polos
    VERSION 0.1.0
    LANGUAGES CXX C
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(STATIC_STD       "Link static libraries of STL" ON)
option(USE_OPENGL       "Use Polos with OpenGL" ON)
option(USE_VULKAN       "Use Polos with Vulkan" OFF)
option(ENABLE_PROFILING "Enable the profiling tools" OFF)
option(GUI_USE_IMGUI    "Use ImGui for default Gui implementation" ON)
option(BUILD_EDITOR     "If not checked, only builds the runtime binaries" OFF)
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
if(NOT ${EXPORT_ENGINE} AND ${BUILD_EDITOR})
    set(INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/Source/Editor")
endif()

set(POLOS_INSTALL_INCLUDES_DIR "${INSTALL_DIR}/include/polos")
set(POLOS_INSTALL_LIBS_DIR "${INSTALL_DIR}")

include(CMake/target_build.cmake)

add_compile_options(
    "$<$<C_COMPILER_ID:Clang>:-fcolor-diagnostics>"
    "$<$<C_COMPILER_ID:Clang>:-fansi-escape-codes>"
    "$<$<C_COMPILER_ID:GNU>:-fdiagnostic s-color=always>"
)

# ================================== #
#       Build the whole source       #
# ================================== #

add_subdirectory(Source)