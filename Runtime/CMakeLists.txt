cmake_minimum_required(VERSION 3.21)

set(Runtime_SOURCES
    engine/core/application.cpp
    engine/core/event_bus.cpp
    engine/core/log.cpp
    engine/engine/engine.cpp
    engine/gui/imgui/imgui_impl_glfw.cpp
    engine/gui/imgui/imgui_impl_opengl3.cpp
    engine/memory/linear_allocator.cpp
    engine/memory/stack_allocator.cpp
    engine/time/scope_timer.cpp
    engine/utils/type_util.cpp
    engine/platform/windows/windows_timer.cpp
    engine/renderer/renderer.cpp
    engine/renderer/context/opengl/opengl_context.cpp
    engine/renderer/context/opengl/opengl_shader_lib.cpp
    engine/renderer/context/opengl/opengl_vao.cpp
    engine/platform/xplat/x_plat_window.cpp
    engine/io/file.cpp
    engine/renderer/context/opengl/opengl_shader.cpp
)

set(Runtime_core_HEADERS
    engine/core/application.h
    engine/core/log.h
    engine/core/event_bus.h
    engine/core/entry_point.h
    engine/core/i_window.h
)

set(Runtime_containers_HEADERS
    engine/containers/delegate.h
    engine/containers/dynamic_array.h
    engine/containers/containers.h
)

set(Runtime_debug_HEADERS
    engine/debug/plassert.h
    engine/debug/profiling.h
)

set(Runtime_engine_HEADERS
    engine/engine/engine.h
)

set(Runtime_events_HEADERS
    engine/events/engine/engine_stop.h
    engine/events/input/key_press.h
    engine/events/input/key_release.h
    engine/events/input/mouse_button_press.h
    engine/events/input/mouse_button_release.h
    engine/events/input/mouse_move.h
    engine/events/input/mouse_scroll.h
    engine/events/window/window_close.h
    engine/events/window/window_focus.h
    engine/events/window/window_framebuffer_size.h
    engine/events/window/window_iconify.h
    engine/events/window/window_maximize.h
    engine/events/window/window_position.h
    engine/events/window/window_resize.h
    engine/events/event.h
)

set(Runtime_gui_HEADERS
    engine/gui/imgui/imgui_impl_glfw.h
    engine/gui/imgui/imgui_impl_opengl3.h
    engine/gui/imgui/imgui_impl_opengl3_loader.h
)

set(Runtime_io_HEADERS
    engine/io/filesystem.h
    engine/io/file.h
)

set(Runtime_memory_HEADERS
    engine/memory/mem_utils.h
    engine/memory/linear_allocator.h
    engine/memory/stack_allocator.h
    engine/memory/pool_allocator.h
)

set(Runtime_time_HEADERS
    engine/time/timer.h
    engine/time/scope_timer.h
)

set(Runtime_utils_HEADERS
    engine/utils/alias.h
    engine/utils/type_util.h
    engine/utils/feature.h
    engine/utils/macro_util.h
    engine/utils/meta.h
    engine/utils/concepts.h
    engine/utils/stringid.h
)

set(Runtime_platform_HEADERS
    engine/platform/platform_detection.h
)

set(Runtime_renderer_HEADERS
    engine/renderer/renderer.h
)

set(Runtime_renderer_context_HEADERS
    engine/renderer/context/graphics_context.h
    engine/renderer/context/shader_lib.h
    engine/renderer/context/vao.h
    engine/renderer/context/vertex.h
    engine/renderer/context/shader.h
)

set(Runtime_window_HEADERS
    engine/platform/xplat/x_plat_window.h
)

set(Runtime_HEADERS
        ${Runtime_core_HEADERS}
        ${Runtime_containers_HEADERS}
        ${Runtime_debug_HEADERS}
        ${Runtime_engine_HEADERS}
        ${Runtime_events_HEADERS}
        ${Runtime_gui_HEADERS}
        ${Runtime_io_HEADERS}
        ${Runtime_memory_HEADERS}
        ${Runtime_time_HEADERS}
        ${Runtime_utils_HEADERS}
        ${Runtime_platform_HEADERS}
        ${Runtime_renderer_HEADERS}
        ${Runtime_renderer_context_HEADERS}
        ${Runtime_window_HEADERS}
        engine/polos_pch.h)

set(EXTRA_LIBS)

list(APPEND EXTRA_LIBS glm_static)

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

add_library(Runtime STATIC ${Runtime_SOURCES} ${Runtime_HEADERS})

target_precompile_headers(Runtime PRIVATE engine/polos_pch.h)

target_compile_features(
    Runtime
    PRIVATE cxx_std_20 cxx_attributes)

target_link_libraries(Runtime PUBLIC ${EXTRA_LIBS})

# for the config header
target_include_directories(Runtime PUBLIC ${PROJECT_BINARY_DIR})

target_include_directories(Runtime
        PUBLIC engine
               engine/core
               engine/platform
               engine/renderer
               engine/window
)

build_options(Runtime true)

#install(TARGETS Runtime DESTINATION bin)