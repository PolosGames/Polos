cmake_minimum_required(VERSION 3.24)

project(
    Polos
    VERSION 0.1.0
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX})

option(STATIC_STD       "Link static libraries of STL" ON)
option(USE_OPENGL       "Use Polos with OpenGL" ON)
option(USE_VULKAN       "Use Polos with Vulkan" OFF)
option(ENABLE_PROFILING "Enable the profiling tools" OFF)
option(ENABLE_LOGGING   "Enable logging inside the engine and in applications" ON)
option(GUI_USE_IMGUI    "Use ImGui for default Gui implementation" ON)
option(BUILD_EDITOR     "If not checked, only builds the runtime binaries" OFF)

include(CMake/target_build.cmake)

# ================================== #
#       Build the whole source       #
# ================================== #

add_compile_options(
    "$<$<C_COMPILER_ID:Clang>:-fcolor-diagnostics>"
    "$<$<C_COMPILER_ID:Clang>:-fansi-escape-codes>"
    "$<$<C_COMPILER_ID:GNU>:-fdiagnostics-color=always>"
)

add_subdirectory(Source)