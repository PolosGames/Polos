cmake_minimum_required(VERSION 3.24)

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
    "polos/containers/containers.h"
    "polos/containers/delegate.h"
    "polos/containers/dynamic_array.h"
    "polos/context/framebuffer.h"
    "polos/context/graphics_context.h"
    "polos/context/shader.h"
    "polos/context/shader_lib.h"
    "polos/context/texture.h"
    "polos/context/vao.h"
    "polos/context/vertex.h"
    "polos/core/ecs/components/camera_component.h"
    "polos/core/ecs/components/transform_component.h"
    "polos/core/ecs/components/texture2d_component.h"
    "polos/core/ecs/components/info_component.h"
    "polos/core/ecs/component.h"
    "polos/core/ecs/component_memory.h"
    "polos/core/ecs/component_pool.h"
    "polos/core/ecs/entity.h"
    "polos/core/scene/base_scene_view.h"
    "polos/core/scene/scene.h"
    "polos/core/scene/scene_data.h"
    "polos/core/scene/scene_export.h"
    "polos/core/scene/scene_loader.h"
    "polos/core/scene/scene_view.h"
    "polos/core/scene/scene_view_iterator.h"
    "polos/core/sound/sound_attributes.h"
    "polos/core/sound/sound_engine.h"
    "polos/core/application.h"
    "polos/core/camera.h"
    "polos/core/event_bus.h"
    "polos/core/log.h"
    "polos/core/update_queue.h"
    "polos/core/window.h"
    "polos/core/window_system.h"
    "polos/debug/plassert.h"
    "polos/debug/profiling.h"
    "polos/engine/engine.h"
    "polos/engine/engine_config.h"
    "polos/events/events.h"
    "polos/events/event.h"
    "polos/gui/gui.h"
    "polos/io/file.h"
    "polos/io/filesystem.h"
    "polos/memory/linear_allocator.h"
    "polos/memory/mem_utils.h"
    "polos/memory/pool_allocator.h"
    "polos/memory/stack_allocator.h"
    "polos/platform/xplat/x_plat_window_callbacks.h"
    "polos/platform/platform_detection.h"
    "polos/renderer/shapes/shapes2d_transform.h"
    "polos/renderer/renderer.h"
    "polos/time/scope_timer.h"
    "polos/time/timer.h"
    "polos/utils/alias.h"
    "polos/utils/concepts.h"
    "polos/utils/feature.h"
    "polos/utils/guid.h"
    "polos/utils/macro_util.h"
    "polos/utils/stringid.h"
    "polos/utils/type_util.h"
)

set(Runtime_SOURCES
    "polos/context/opengl/gl_context.cpp"
    "polos/context/opengl/gl_shader_lib.cpp"
    "polos/context/opengl/gl_vao.cpp"
    "polos/context/opengl/gl_shader.cpp"
    "polos/context/opengl/gl_texture.cpp"
    "polos/context/opengl/gl_framebuffer.cpp"
    "polos/core/ecs/component_memory.cpp"
    "polos/core/ecs/component_pool.cpp"
    "polos/core/scene/scene.cpp"
    "polos/core/scene/scene_view_iterator.cpp"
    "polos/core/application.cpp"
    "polos/core/event_bus.cpp"
    "polos/core/log.cpp"
    "polos/core/camera.cpp"
    "polos/core/update_queue.cpp"
    "polos/core/window_system.cpp"
    "polos/core/sound/sound_engine.cpp"
    "polos/engine/engine.cpp"
    "polos/gui/imgui/imgui_gui.cpp"
    "polos/io/file.cpp"
    "polos/memory/linear_allocator.cpp"
    "polos/memory/stack_allocator.cpp"
    "polos/time/scope_timer.cpp"
    "polos/platform/windows/windows_timer.cpp"
    "polos/platform/linux/linux_timer.cpp"
    "polos/platform/xplat/x_plat_window_system.cpp"
    "polos/platform/xplat/x_plat_window_callbacks.cpp"
    "polos/renderer/shapes/shapes2d_transform.cpp"
    "polos/renderer/renderer.cpp"
    "polos/utils/type_util.cpp"
    "polos/utils/stringid.cpp"
    "polos/utils/guid.cpp"
 "polos/core/ecs/component.cpp" "polos/core/scene/scene_loader.cpp")

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
)

build_options(Runtime true)
