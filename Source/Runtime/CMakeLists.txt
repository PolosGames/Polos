cmake_minimum_required(VERSION 3.21)

set(EXTRA_LIBS)

list(APPEND EXTRA_LIBS glm_static)
list(APPEND EXTRA_LIBS stb_image)
list(APPEND EXTRA_LIBS stb_textedit)
list(APPEND EXTRA_LIBS RapidJSON)

if(USE_OPENGL OR USE_VULKAN)
    list(APPEND EXTRA_LIBS glfw)
endif()

if(USE_OPENGL)
    list(APPEND EXTRA_LIBS glad)
endif()

if(ENABLE_PROFILING)
    list(APPEND EXTRA_LIBS Optick)
endif()

if(ENABLE_LOGGING)
    list(APPEND EXTRA_LIBS spdlog)
endif()

list(APPEND EXTRA_LIBS ImGui)

if (UNIX)
    list(APPEND EXTRA_LIBS pthread)
endif (UNIX)

set(Runtime_SOURCES
    core/application.cpp
    core/event_bus.cpp
    core/log.cpp
    core/camera.cpp
    core/update_queue.cpp
    core/resource_manager.cpp
    engine/engine.cpp
    gui/imgui/imgui_impl_glfw.cpp
    gui/imgui/imgui_impl_opengl3.cpp
    gui/imgui/imgui_gui.cpp
    memory/linear_allocator.cpp
    memory/stack_allocator.cpp
    time/scope_timer.cpp
    utils/type_util.cpp
    platform/windows/windows_timer.cpp
    platform/linux/linux_timer.cpp
    renderer/renderer.cpp
    renderer/context/opengl/opengl_context.cpp
    renderer/context/opengl/opengl_shader_lib.cpp
    renderer/context/opengl/opengl_vao.cpp
    renderer/context/opengl/opengl_shader.cpp
    renderer/context/opengl/opengl_texture.cpp
    platform/xplat/x_plat_window.cpp
    io/file.cpp
)

add_library(Runtime STATIC)

set_target_properties(Runtime PROPERTIES OUTPUT_NAME "PolosRuntime")

target_sources(Runtime
               PRIVATE ${Runtime_SOURCES}
)

target_precompile_headers(Runtime
                          PUBLIC polos_pch.h)

target_compile_features(Runtime
                        PRIVATE cxx_std_20 cxx_attributes)

target_link_libraries(Runtime
                      PUBLIC ${EXTRA_LIBS})

# for the config header
target_include_directories(Runtime
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} 
                                  ${PROJECT_BINARY_DIR}
                           INTERFACE include
)

target_compile_definitions(Runtime PUBLIC GLFW_INCLUDE_NONE)

build_options(Runtime true)

#install(TARGETS Runtime DESTINATION bin)
install(DIRECTORY resources DESTINATION ${INSTALL_DIR})