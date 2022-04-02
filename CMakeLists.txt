cmake_minimum_required(VERSION 3.21)

project(
    Polos
    VERSION 0.1.0
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

option(USE_OPENGL "Use Polos with OpenGL" ON)
option(USE_VULKAN "Use Polos with Vulkan" OFF)
option(ENABLE_PROFILING "Enable the profiling tools" OFF)
option(ENABLE_LOGGING "Enable logging inside the engine and in applications" ON)

include(CMake/target_build.cmake)

add_compile_definitions($<$<CONFIG:Debug,RelWithDebInfo>:PL_DEBUG>
                        $<$<CONFIG:Release,MinSizeRel>:PL_RELEASE>)

# ================================== #
#  Add the third parties into build  #
# ================================== #

add_subdirectory(ThirdParty/glm)

if(USE_OPENGL OR USE_VULKAN)
    add_subdirectory(ThirdParty/GLFW)
    set_target_properties(glfw PROPERTIES FOLDER "ThirdParty")
endif()

if(USE_OPENGL)
    add_compile_definitions(USE_OPENGL)
    add_subdirectory(ThirdParty/glad)
    set_target_properties(glad PROPERTIES FOLDER "ThirdParty")
endif()

if(USE_VULKAN)
    add_compile_definitions(USE_VULKAN)
endif()

if(ENABLE_PROFILING)
    add_compile_definitions(ENABLE_PROFILING)
    add_subdirectory(ThirdParty/Optick)
    set_target_properties(Optick PROPERTIES FOLDER "ThirdParty")
endif()

if(ENABLE_LOGGING)
    add_compile_definitions(ENABLE_LOGGING)
    add_subdirectory(ThirdParty/spdlog)
    set_target_properties(spdlog PROPERTIES FOLDER "ThirdParty")
endif()

configure_file(plconfig.h.in plconfig.h)

# ================================== #
#     Add the runtime into build     #
# ================================== #

add_subdirectory(Runtime)

# ================================== #
#             Build Polos            #
# ================================== #

add_executable(Polos Sandbox/sandbox_app.cpp)

target_compile_features(Polos PRIVATE cxx_std_20)

target_link_libraries(Polos PUBLIC Runtime)

build_options(Polos true)

install(TARGETS Polos DESTINATION bin)