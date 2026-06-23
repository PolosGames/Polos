///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_PASSES_GENERAL_PASS_HPP
#define POLOS_RENDERING_PASSES_GENERAL_PASS_HPP

#include "polos/rendering/vertex.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace polos::rendering
{

class RenderContext;
class VulkanSwapchain;
class VulkanResourceManager;
class ShaderCache;
class PipelineCache;

class GeneralPass
{
public:
    explicit GeneralPass(RenderContext& t_context);
    ~GeneralPass();

    auto Execute(VkCommandBuffer t_cmd_buf, std::uint32_t t_current_frame) -> void;
    void OnResize();
private:
    VkRenderPass     createRenderPass();
    VkImageView      createDrawImageView();
    VkFramebuffer    createFramebuffer();
    VkPipeline       createPipeline();
    VkBuffer         createVertexBuffer();
    VkBuffer         createIndexBuffer();
    void             createUboMapping();
    VkDescriptorPool createDescriptorPool();
    void             createDescriptorSets();
    VkImageView      createTexture();
    VkSampler        createTextureSampler();
    void             createDepthResources();

    RenderContext&         m_context;
    VulkanSwapchain*       m_swapchain{nullptr};
    VulkanResourceManager* m_vrm{nullptr};
    ShaderCache*           m_shader_cache{nullptr};
    PipelineCache*         m_pipeline_cache{nullptr};

    VkCommandBuffer m_command_buffer{VK_NULL_HANDLE};

    VkPipeline                 m_pipeline{VK_NULL_HANDLE};
    std::int32_t               m_draw_image_index{0};
    VkImage                    m_draw_image{VK_NULL_HANDLE};
    VkImageView                m_draw_image_view{VK_NULL_HANDLE};
    VkDevice                   m_device{VK_NULL_HANDLE};
    std::vector<VkFramebuffer> m_pass_fb;
    std::int32_t               m_buffer_vertices_index{0};
    VkBuffer                   m_buffer_vertices{VK_NULL_HANDLE};
    std::int32_t               m_buffer_indices_index{0};
    VkBuffer                   m_buffer_indices{VK_NULL_HANDLE};
    std::vector<std::int32_t>  m_buffer_instancing_indices;
    std::vector<void*>         m_instance_mappings;
    std::vector<Vertex>        m_vertices;
    std::vector<std::uint16_t> m_indices;
    std::vector<std::int32_t>  m_buffer_indices_ubos;
    std::vector<void*>         m_ubo_mappings;
    std::int32_t               m_texture_image_index{0};
    VkImageView                m_image_view_tux_texture{VK_NULL_HANDLE};
    VkSampler                  m_sampler_tux_texture{VK_NULL_HANDLE};
    VkImage                    m_depth_image{VK_NULL_HANDLE};
    VkImageView                m_depth_image_view{VK_NULL_HANDLE};
    std::int32_t               m_depth_image_index{0};

    VkDescriptorPool                   m_descriptor_pool{VK_NULL_HANDLE};
    std::vector<VkDescriptorSet>       m_descriptor_sets;
    std::vector<VkDescriptorSetLayout> m_descriptor_set_layouts;

    VkRenderPass m_render_pass{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_PASSES_GENERAL_PASS_HPP
