cmake_minimum_required(VERSION 3.21)

set(Runtime_SOURCES
    "engine/core/application.cpp"
    "engine/core/event_bus.cpp"
    "engine/core/log.cpp"
    "engine/core/engine/engine.cpp"
    "engine/core/memory/linear_allocator.cpp"
    "engine/core/memory/stack_allocator.cpp"
    "engine/core/time/scope_timer.cpp"
    "engine/core/utils/type_util.cpp"
    "engine/platform/windows/windows_timer.cpp"
    "engine/renderer/renderer.cpp"
    "engine/renderer/context/opengl/opengl_context.cpp"
    "engine/renderer/context/opengl/opengl_shader.cpp"
    "engine/renderer/context/opengl/opengl_vbo.cpp"
    "engine/window/x_plat_window.cpp"
)

set(Runtime_core_HEADERS
    engine/core/application.h
    engine/core/log.h
    engine/core/event_bus.h
    engine/core/entry_point.h
)

set(Runtime_core_containers_HEADERS
    engine/core/containers/delegate.h
    engine/core/containers/dynamic_array.h
    engine/core/containers/containers.h)

set(Runtime_core_debug_HEADERS
    engine/core/debug/plassert.h
    engine/core/debug/profiling.h
)

set(Runtime_core_engine_HEADERS
    engine/core/engine/engine.h
)

set(Runtime_core_events_HEADERS
    engine/core/events/engine/engine_stop.h
    engine/core/events/input/key_press.h
    engine/core/events/input/key_release.h
    engine/core/events/input/mouse_button_press.h
    engine/core/events/input/mouse_button_release.h
    engine/core/events/input/mouse_move.h
    engine/core/events/input/mouse_scroll.h
    engine/core/events/window/window_close.h
    engine/core/events/window/window_focus.h
    engine/core/events/window/window_framebuffer_size.h
    engine/core/events/window/window_iconify.h
    engine/core/events/window/window_maximize.h
    engine/core/events/window/window_position.h
    engine/core/events/window/window_resize.h
    engine/core/events/event.h
)

set(Runtime_core_memory_HEADERS
    engine/core/memory/mem_utils.h
    engine/core/memory/linear_allocator.h
    engine/core/memory/stack_allocator.h
    engine/core/memory/pool_allocator.h
)

set(Runtime_core_time_HEADERS
    engine/core/time/timer.h
    engine/core/time/scope_timer.h
)

set(Runtime_core_utils_HEADERS
    engine/core/utils/alias.h
    engine/core/utils/type_util.h
    engine/core/utils/feature.h
    engine/core/utils/macro_util.h
    engine/core/utils/meta.h
    engine/core/utils/concepts.h)

set(Runtime_platform_HEADERS
    engine/platform/platform_detection.h
)

set(Runtime_renderer_HEADERS
    engine/renderer/renderer.h
)

set(Runtime_renderer_context_HEADERS
    engine/renderer/context/graphics_context.h
    engine/renderer/context/shader.h
    engine/renderer/context/vbo.h
)

set(Runtime_window_HEADERS
    engine/window/x_plat_window.h
)

set(Runtime_HEADERS
        ${Runtime_core_HEADERS}
        ${Runtime_core_containers_HEADERS}
        ${Runtime_core_debug_HEADERS}
        ${Runtime_core_engine_HEADERS}
        ${Runtime_core_events_HEADERS}
        ${Runtime_core_memory_HEADERS}
        ${Runtime_core_time_HEADERS}
        ${Runtime_core_utils_HEADERS}
        ${Runtime_platform_HEADERS}
        ${Runtime_renderer_HEADERS}
        ${Runtime_renderer_context_HEADERS}
        ${Runtime_window_HEADERS}
        engine/polos_pch.h)

set(EXTRA_LIBS)

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

add_library(Runtime STATIC ${Runtime_SOURCES} ${Runtime_HEADERS})

target_precompile_headers(Runtime PRIVATE engine/polos_pch.h)

target_compile_features(
    Runtime
    PRIVATE cxx_std_20 cxx_attributes)

target_link_libraries(Runtime PUBLIC ${EXTRA_LIBS})

# for the config header
target_include_directories(Runtime PUBLIC ${PROJECT_BINARY_DIR})

target_include_directories(Runtime
        PUBLIC "engine"
               "engine/core"
               "engine/platform"
               "engine/renderer"
               "engine/window"
)

build_options(Runtime)

#install(TARGETS Runtime DESTINATION bin)