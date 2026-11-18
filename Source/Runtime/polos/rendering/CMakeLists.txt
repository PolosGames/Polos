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
    resources/material.cpp
    resources/shader.cpp
    resources/texture.cpp
    ssbo/quad_instance.cpp
    uniforms/camera_uniform.cpp
    vertex/mesh_vertex.cpp
    vertex/quad_vertex.cpp
    vk/command_buffer.cpp
    vk/compute_pipeline_builder.cpp
    vk/dependency_info_builder.cpp
    vk/descriptor_pool.cpp
    vk/descriptor_pool_builder.cpp
    vk/graphics_pipeline_builder.cpp
    vk/render_context.cpp
    vk/render_pass_builder.cpp
    vk/vulkan_context.cpp
    vk/vulkan_device.cpp
    vk/vulkan_pipeline.cpp
    vk/vulkan_render_pass.cpp
    vk/vulkan_swapchain.cpp
    vk/vulkan_utils.cpp
    vulkan_rendering_api.cpp
)

polos_tests(rendering SOURCES
    test/render_subsystem_test.cpp
    test/vertex_layout_test.cpp
    test/camera_uniform_test.cpp
    vertex/mesh_vertex.cpp
    vertex/quad_vertex.cpp
)
