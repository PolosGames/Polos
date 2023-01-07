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

list (APPEND EXTRA_LIBS FMOD)
list (APPEND EXTRA_LIBS FMODL)

set(Runtime_HEADERS
    polos/containers/containers.h
    polos/containers/delegate.h
    polos/containers/dynamic_array.h
    polos/core/log.h
    polos/core/update_queue.h
)

set(Runtime_SOURCES
    polos/core/application.cpp
    polos/core/event_bus.cpp
    polos/core/log.cpp
    polos/core/camera.cpp
    polos/core/update_queue.cpp
    polos/core/window_system.cpp
    polos/engine/engine.cpp
    polos/gui/imgui/imgui_impl_glfw.cpp
    polos/gui/imgui/imgui_impl_opengl3.cpp
    polos/gui/imgui/imgui_gui.cpp
    polos/memory/linear_allocator.cpp
    polos/memory/stack_allocator.cpp
    polos/time/scope_timer.cpp
    polos/utils/type_util.cpp
    polos/utils/stringid.cpp
    polos/utils/guid.cpp
    polos/platform/windows/windows_timer.cpp
    polos/platform/linux/linux_timer.cpp
    polos/renderer/renderer.cpp
    polos/context/opengl/gl_context.cpp
    polos/context/opengl/gl_shader_lib.cpp
    polos/context/opengl/gl_vao.cpp
    polos/context/opengl/gl_shader.cpp
    polos/context/opengl/gl_texture.cpp
    polos/platform/xplat/x_plat_window_system.cpp
    polos/platform/xplat/x_plat_window_callbacks.cpp
    polos/io/file.cpp
    polos/core/sound/sound_engine.cpp
    polos/renderer/shapes/shapes2d_transform.cpp
)

add_library(Runtime STATIC)

set_target_properties(
    Runtime
        PROPERTIES
            OUTPUT_NAME "PolosRuntime"
)

target_sources(Runtime
    PRIVATE
        ${Runtime_SOURCES}
        ${Runtime_HEADERS}
)

target_precompile_headers(
    Runtime
        PUBLIC
            polos/polos_pch.h
)

set_target_properties(
    Runtime
        PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED True
)

target_link_libraries(
    Runtime
        PUBLIC
            ${EXTRA_LIBS}
)

target_include_directories(
    Runtime
        PRIVATE
            ${PROJECT_BINARY_DIR}
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}
            ../ThirdParty/fmod/inc
)

target_compile_definitions(
    Runtime
        PUBLIC
            GLFW_INCLUDE_NONE
            _CRT_SECURE_NO_WARNINGS
            $<$<CONFIG:Debug,RelWithDebInfo>:PL_DEBUG>
            $<$<CONFIG:Release,MinSizeRel>:PL_RELEASE>
)

build_options(Runtime true)
