cmake_minimum_required(VERSION 3.24)

project(
    Polos
    VERSION 0.1.0
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(STATIC_STD       "Link static libraries of STL" ON)
option(USE_OPENGL       "Use Polos with OpenGL" ON)
option(USE_VULKAN       "Use Polos with Vulkan" OFF)
option(ENABLE_PROFILING "Enable the profiling tools" OFF)
option(ENABLE_LOGGING   "Enable logging inside the engine and in applications" ON)
option(GUI_USE_IMGUI    "Use ImGui for default Gui implementation" ON)
option(BUILD_EDITOR     "If not checked, only builds the runtime binaries" OFF)
option(EXPORT_ENGINE    "Will install everything under bin folder." OFF)

set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
if(NOT ${EXPORT_ENGINE} AND ${BUILD_EDITOR})
    set(INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/Source/Editor")
endif()

include(CMake/target_build.cmake)

# ================================== #
#       Build the whole source       #
# ================================== #

add_compile_options(
    "$<$<C_COMPILER_ID:Clang>:-fcolor-diagnostics>"
    "$<$<C_COMPILER_ID:Clang>:-fansi-escape-codes>"
    "$<$<C_COMPILER_ID:GNU>:-fdiagnostic s-color=always>"
)

add_subdirectory(Source)