cmake_minimum_required(VERSION 3.25)

polos_sources(
    cache/material_cache.cpp
    cache/mesh_cache.cpp
    cache/texture_cache.cpp
    compositor/render_compositor.cpp
    passes/forward_shading_pass.cpp
    passes/ui_render_pass.cpp
    resources/buffer.cpp
    resources/image.cpp
    resources/shader.cpp
    resources/texture.cpp
    ssbo/quad_instance.cpp
    uniforms/camera_uniform.cpp
    vertex/primitive_vertex.cpp
    vertex/quad_vertex.cpp
    vk/descriptor_pool.cpp
    vk/descriptor_set.cpp
    vk/descriptor_set_layout.cpp
    vk/render_context.cpp
    vk/vulkan_context.cpp
    vk/vulkan_device.cpp
    vk/vulkan_pipeline.cpp
    vk/vulkan_swapchain.cpp
    vk/vulkan_utils.cpp
    vulkan_rendering_api.cpp
)

polos_tests(rendering SOURCES
    test/render_subsystem_test.cpp
    test/vertex_layout_test.cpp
    test/camera_uniform_test.cpp
    vertex/primitive_vertex.cpp
    vertex/quad_vertex.cpp
)
