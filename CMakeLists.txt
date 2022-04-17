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

option(STATIC_STD "Link static libraries of STL" ON)
option(USE_OPENGL "Use Polos with OpenGL" ON)
option(USE_VULKAN "Use Polos with Vulkan" OFF)
option(ENABLE_PROFILING "Enable the profiling tools" OFF)
option(ENABLE_LOGGING "Enable logging inside the engine and in applications" ON)
option(GUI_USE_IMGUI "Use ImGui for default Gui implementation" ON)

include(CMake/target_build.cmake)

add_compile_definitions($<$<CONFIG:Debug,RelWithDebInfo>:PL_DEBUG>
                        $<$<CONFIG:Release,MinSizeRel>:PL_RELEASE>)

# ================================== #
#  Add the third parties into build  #
# ================================== #

add_subdirectory(ThirdParty/glm)
set_target_properties(glm_static PROPERTIES FOLDER "ThirdParty")

add_subdirectory(ThirdParty/stb_image)
set_target_properties(stb_image  PROPERTIES FOLDER "ThirdParty")

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

if(GUI_USE_IMGUI)
    add_subdirectory(ThirdParty/imgui)
    set_target_properties(ImGui PROPERTIES FOLDER "ThirdParty")
    add_compile_definitions(GUI_USE_IMGUI)
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
if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Polos)
endif()

target_compile_features(Polos PRIVATE cxx_std_20)

set(EXTRA_LIBS)
if(STATIC_STD)
    list(APPEND EXTRA_LIBS -static-libstdc++)
endif()

target_link_libraries(Polos
                      PUBLIC Runtime
                             ${EXTRA_LIBS}
)

build_options(Polos true)

install(TARGETS Polos DESTINATION bin/${CMAKE_BUILD_TYPE})