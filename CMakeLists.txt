cmake_minimum_required(VERSION 3.21)

project(
    Polos
    VERSION 0.1.0
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR} CACHE PATH "..." FORCE)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR} CACHE PATH "..." FORCE)
endif()

set(INSTALL_DIR bin/${CMAKE_BUILD_TYPE})

option(STATIC_STD "Link static libraries of STL" ON)
option(USE_OPENGL "Use Polos with OpenGL" ON)
option(USE_VULKAN "Use Polos with Vulkan" OFF)
option(ENABLE_PROFILING "Enable the profiling tools" OFF)
option(ENABLE_LOGGING "Enable logging inside the engine and in applications" ON)
option(GUI_USE_IMGUI "Use ImGui for default Gui implementation" ON)
option(BUILD_EDITOR "If not checked, only builds the runtime binaries" ON)

include(CMake/target_build.cmake)

add_compile_definitions($<$<CONFIG:Debug,RelWithDebInfo>:PL_DEBUG>
                        $<$<CONFIG:Release,MinSizeRel>:PL_RELEASE>)

configure_file(plconfig.h.in plconfig.h)

# ================================== #
#       Build the whole source       #
# ================================== #

add_subdirectory(Source)