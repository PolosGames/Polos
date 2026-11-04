cmake_minimum_required(VERSION 3.25)

polos_sources(
    cache/pipeline_cache.cpp
    cache/shader_cache.cpp
    cache/texture_cache.cpp
    compositor/render_compositor.cpp
    #passes/general_pass.cpp
    passes/opaque_geometry_pass.cpp
    resources/buffer.cpp
    resources/image.cpp
    resources/texture.cpp
    ssbo/quad_instance.cpp
    uniforms/camera_uniform.cpp
    vertex/primitive_vertex.cpp
    vertex/quad_vertex.cpp
    vk/render_context.cpp
    vk/vulkan_context.cpp
    vk/vulkan_device.cpp
    vk/vulkan_swapchain.cpp
    vk/vulkan_util.cpp
    vulkan_rendering_api.cpp
)

polos_tests(rendering SOURCES
    test/render_subsystem_test.cpp
    test/vertex_layout_test.cpp
    vertex/primitive_vertex.cpp
    vertex/quad_vertex.cpp
)
