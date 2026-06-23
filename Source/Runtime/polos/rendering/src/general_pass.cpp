///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/passes/general_pass.hpp"

#include "polos/rendering/common.hpp"
#include "polos/rendering/pipeline_cache.hpp"
#include "polos/rendering/quad_instance.hpp"
#include "polos/rendering/render_context.hpp"
#include "polos/rendering/render_object.hpp"
#include "polos/rendering/render_pass_layout_description.hpp"
#include "polos/rendering/rendering_api.hpp"
#include "polos/rendering/scene.hpp"
#include "polos/rendering/shader_cache.hpp"
#include "polos/rendering/texture_description.hpp"
#include "polos/rendering/uniform_buffer_object.hpp"
#include "polos/rendering/vulkan_device.hpp"
#include "polos/rendering/vulkan_resource_manager.hpp"
#include "polos/rendering/vulkan_swapchain.hpp"
#include "polos/rendering/vulkan_util.hpp"
#include "polos/utils/string_id.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

namespace polos::rendering
{

static constexpr std::uint64_t kMaxQuadInstances = 10000U;

GeneralPass::GeneralPass(RenderContext& t_context)
    : m_context(t_context),
      m_swapchain(&m_context.GetSwapchain()),
      m_vrm(&m_context.GetVulkanResourceManager()),
      m_shader_cache(&m_context.GetShaderCache()),
      m_pipeline_cache(&m_context.GetPipelineCache()),
      m_device(m_context.GetVulkanDevice().logi_device)
{
    createDepthResources();
    m_render_pass     = createRenderPass();
    m_draw_image_view = createDrawImageView();

    std::size_t const frames_in_flight = m_context.GetFramesInFlight();
    m_pass_fb.resize(frames_in_flight);
    for (std::uint32_t i{0U}; i < frames_in_flight; ++i) { m_pass_fb[i] = createFramebuffer(); }

    m_pipeline = createPipeline();

    m_vertices = {
        // RGB Triangle
        Vertex{
            .position  = {-0.5F, -0.5F, 0.0F},
            .color     = {1.0F, 0.0F, 0.0F},
            .tex_coord = {0.0F, 0.0F},
        },
        Vertex{
            .position  = {0.5F, -0.5F, 0.0F},
            .color     = {0.0F, 1.0F, 0.0F},
            .tex_coord = {1.0F, 0.0F},
        },
        Vertex{
            .position  = {0.5F, 0.5F, 0.0F},
            .color     = {0.0F, 0.0F, 1.0F},
            .tex_coord = {1.0F, 1.0F},
        },
        Vertex{
            .position  = {-0.5F, 0.5F, 0.0F},
            .color     = {1.0F, 1.0F, 1.0F},
            .tex_coord = {0.0F, 1.0F},
        },
    };

    m_indices = {
        0U,
        1U,
        2U,
        2U,
        3U,
        0U,
    };

    m_buffer_vertices = createVertexBuffer();
    m_buffer_indices  = createIndexBuffer();

    m_image_view_tux_texture = createTexture();
    m_sampler_tux_texture    = createTextureSampler();

    m_buffer_indices_ubos.resize(frames_in_flight);
    m_buffer_instancing_indices.resize(frames_in_flight);
    m_instance_mappings.resize(frames_in_flight);
    m_ubo_mappings.resize(frames_in_flight);
    createUboMapping();

    m_descriptor_sets.resize(frames_in_flight);
    m_descriptor_pool = createDescriptorPool();
    createDescriptorSets();
}

GeneralPass::~GeneralPass()
{
    for (std::size_t i{0}; i < m_buffer_indices_ubos.size(); ++i)
    {
        vmaUnmapMemory(m_context.GetVulkanDevice().allocator, m_vrm->GetBuffer(m_buffer_indices_ubos[i])->allocation);
        m_vrm->DestroyBuffer(m_buffer_indices_ubos[i]);
    }
    for (std::size_t i{0}; i < m_buffer_instancing_indices.size(); ++i)
    {
        vmaUnmapMemory(m_context.GetVulkanDevice().allocator,
                       m_vrm->GetBuffer(m_buffer_instancing_indices[i])->allocation);
        m_vrm->DestroyBuffer(m_buffer_instancing_indices[i]);
    }
    for (auto& fbuf : m_pass_fb) { vkDestroyFramebuffer(m_device, fbuf, nullptr); }

    vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);
    vkDestroyImageView(m_device, m_draw_image_view, nullptr);
    m_vrm->DestroyImage(m_draw_image_index);

    vkDestroyImageView(m_device, m_depth_image_view, nullptr);
    m_vrm->DestroyImage(m_depth_image_index);

    vkDestroySampler(m_device, m_sampler_tux_texture, nullptr);
    vkDestroyImageView(m_device, m_image_view_tux_texture, nullptr);
    m_vrm->DestroyImage(m_texture_image_index);

    m_vrm->DestroyBuffer(m_buffer_vertices_index);
    m_vrm->DestroyBuffer(m_buffer_indices_index);

    vkDestroyRenderPass(m_device, m_render_pass, nullptr);
}

auto GeneralPass::Execute(VkCommandBuffer t_cmd_buf, std::uint32_t t_current_frame) -> void
{
    m_command_buffer = t_cmd_buf;

    std::span<RenderObject> const render_objects = RenderingApi::GetMainScene()->GetObjects();

    std::array<VkClearValue, 2U> clear_color{
        VkClearValue{
            .color = {.float32 = {common::kPolosRed, common::kPolosGreen, common::kPolosBlue, 1.0F}},
        },
        VkClearValue{
            .depthStencil = {.depth = 1.0F, .stencil = 0U},
        },
    };

    UniformBufferObject ubo{};
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(60.0f),
                                static_cast<float>(m_swapchain->GetExtent().width) /
                                    static_cast<float>(m_swapchain->GetExtent().height),
                                0.1f,
                                10.0f);
    ubo.proj[1][1] *= -1;

    std::size_t current_instance_count = std::min(render_objects.size(), kMaxQuadInstances);

    if (current_instance_count < render_objects.size())
    {
        // TODO(sorbatdev): Log once
        LogWarn(
            "Number of render objects ({}) exceeds maximum quad instances ({}). Only rendering the first {} objects.",
            render_objects.size(),
            kMaxQuadInstances,
            current_instance_count);
    }

    //NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    std::vector<QuadInstance> instance_data(current_instance_count);
    for (std::size_t i{0U}; i < current_instance_count; ++i) { instance_data[i].model = render_objects[i].transform; }

    std::memcpy(m_instance_mappings[t_current_frame],
                instance_data.data(),
                sizeof(QuadInstance) * current_instance_count);
    std::memcpy(m_ubo_mappings[t_current_frame], &ubo, sizeof(ubo));

    VkRenderPassBeginInfo const pass_begin_info{
        .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext       = nullptr,
        .renderPass  = m_render_pass,
        .framebuffer = m_pass_fb[t_current_frame],
        .renderArea =
            {
                .offset = VkOffset2D{.x = 0, .y = 0},
                .extent = m_swapchain->GetExtent(),
            },
        .clearValueCount = VK_SIZE_CAST(clear_color.size()),
        .pClearValues    = clear_color.data(),
    };

    util::TransitionImageLayout(t_cmd_buf,
                                m_vrm->GetImage(m_depth_image_index)->image,
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT);

    // clang-format off
        // No transition needed as render pass implicitly transitions from:
        // UNDEFINED -> COLOR_ATTACHMENT_OPTIMAL at the start of the render pass
        // and COLOR_ATTACHMENT_OPTIMAL -> TRANSFER_SRC_OPTIMAL at the end of the render pass

        vkCmdBeginRenderPass(t_cmd_buf, &pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdSetViewport(t_cmd_buf, 0U, 1U, &m_swapchain->GetViewport());
            vkCmdSetScissor(t_cmd_buf, 0U, 1U, &m_swapchain->GetScissor());
            vkCmdBindPipeline(t_cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

            // Bind vertex buffer
            VkDeviceSize const offset{0U};
            vkCmdBindVertexBuffers(t_cmd_buf, 0U, 1U, &m_buffer_vertices, &offset);
            vkCmdBindIndexBuffer(t_cmd_buf, m_buffer_indices, offset, VK_INDEX_TYPE_UINT16);
            vkCmdBindDescriptorSets(
                t_cmd_buf,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_pipeline_cache->GetPipeline("pl_basiccolor"_sid)->layout,
                0U, 1U, &m_descriptor_sets[t_current_frame], 0U, nullptr);
            vkCmdDrawIndexed(t_cmd_buf, VK_SIZE_CAST(m_indices.size()), VK_SIZE_CAST(current_instance_count), 0U, 0U, 0U);
        vkCmdEndRenderPass(t_cmd_buf);

    // clang-format on

    util::TransitionImageLayout(t_cmd_buf,
                                m_swapchain->GetCurrentImage(),
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                VK_PIPELINE_STAGE_TRANSFER_BIT);

    util::CopyImageToImage(t_cmd_buf,
                           m_draw_image,
                           m_swapchain->GetCurrentImage(),
                           m_vrm->GetImage(m_draw_image_index)->extent,
                           m_swapchain->GetExtent3D());

    util::TransitionImageLayout(t_cmd_buf,
                                m_swapchain->GetCurrentImage(),
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
}

void GeneralPass::OnResize()
{
    vkDeviceWaitIdle(m_device);

    // Destroy old framebuffers
    for (auto& fbuf : m_pass_fb) { vkDestroyFramebuffer(m_device, fbuf, nullptr); }

    // Destroy old draw image view and image
    vkDestroyImageView(m_device, m_draw_image_view, nullptr);
    m_vrm->DestroyImage(m_draw_image_index);

    // Recreate draw image and framebuffers with new swapchain extent
    m_draw_image_view = createDrawImageView();

    // Recreate depth resources with new swapchain extent
    vkDestroyImageView(m_device, m_depth_image_view, nullptr);
    m_vrm->DestroyImage(m_depth_image_index);
    createDepthResources();

    std::size_t const frames_in_flight = m_swapchain->GetImageCount();
    m_pass_fb.resize(frames_in_flight);
    for (std::uint32_t i{0U}; i < frames_in_flight; ++i) { m_pass_fb[i] = createFramebuffer(); }
}

VkRenderPass GeneralPass::createRenderPass()
{
    static constexpr VkAttachmentReference2 const kColorAttachmentRef{
        .sType      = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
        .pNext      = nullptr,
        .attachment = 0U,
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    };

    static constexpr VkAttachmentReference2 const kDepthAttachmentRef{
        .sType      = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
        .pNext      = nullptr,
        .attachment = 1U,
        .layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
    };

    constexpr VkSubpassDescription2 const kSubpassDesc{
        .sType                   = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .viewMask                = 0U,
        .inputAttachmentCount    = 0U,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = 1U,
        .pColorAttachments       = &kColorAttachmentRef,
        .pResolveAttachments     = nullptr,
        .pDepthStencilAttachment = &kDepthAttachmentRef,
        .preserveAttachmentCount = 0U,
        .pPreserveAttachments    = nullptr,
    };

    constexpr VkSubpassDependency2 const kSubpassDependency{
        .sType           = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
        .pNext           = nullptr,
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0U,
        .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0U,
        .viewOffset      = 0U,
    };

    RenderPassLayoutDescription layout{
        .attachments =
            {
                VkAttachmentDescription2{
                    .sType          = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
                    .pNext          = nullptr,
                    .flags          = 0U,
                    .format         = VK_FORMAT_R16G16B16A16_SFLOAT,
                    .samples        = VK_SAMPLE_COUNT_1_BIT,
                    .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
                    .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
                    .finalLayout    = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                },
                VkAttachmentDescription2{
                    .sType          = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
                    .pNext          = nullptr,
                    .flags          = 0U,
                    .format         = VK_FORMAT_D32_SFLOAT,
                    .samples        = VK_SAMPLE_COUNT_1_BIT,
                    .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
                    .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                },
            },
        .subpasses    = {kSubpassDesc},
        .dependencies = {kSubpassDependency},
    };

    VkRenderPassCreateInfo2 const pass_info{
        .sType                   = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .attachmentCount         = VK_SIZE_CAST(layout.attachments.size()),
        .pAttachments            = layout.attachments.data(),
        .subpassCount            = VK_SIZE_CAST(layout.subpasses.size()),
        .pSubpasses              = layout.subpasses.data(),
        .dependencyCount         = VK_SIZE_CAST(layout.dependencies.size()),
        .pDependencies           = layout.dependencies.data(),
        .correlatedViewMaskCount = 0U,
        .pCorrelatedViewMasks    = nullptr,
    };

    VkRenderPass render_pass{VK_NULL_HANDLE};
    assert(vkCreateRenderPass2(m_device, &pass_info, nullptr, &render_pass) == VK_SUCCESS);

    return render_pass;
}

VkImageView GeneralPass::createDrawImageView()
{
    VkImageUsageFlags usage{0U};
    usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkExtent3D const extent{
        .width  = m_swapchain->GetExtent().width,
        .height = m_swapchain->GetExtent().height,
        .depth  = 1U,
    };

    auto res = m_vrm->CreateImage(
        VkImageCreateInfo{
            .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext                 = nullptr,
            .flags                 = 0U,
            .imageType             = VK_IMAGE_TYPE_2D,
            .format                = VK_FORMAT_R16G16B16A16_SFLOAT,
            .extent                = extent,
            .mipLevels             = 1U,
            .arrayLayers           = 1U,
            .samples               = VK_SAMPLE_COUNT_1_BIT,
            .tiling                = VK_IMAGE_TILING_OPTIMAL,
            .usage                 = usage,
            .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0U,
            .pQueueFamilyIndices   = nullptr,
            .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
        },
        0U,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    assert(res.has_value());
    m_draw_image_index = *res;

    m_draw_image = m_vrm->GetImage(m_draw_image_index)->image;

    return util::CreateImageView(m_device, m_draw_image, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);
}

VkFramebuffer GeneralPass::createFramebuffer()
{
    std::array<VkImageView, 2U> const attachment_views{
        m_draw_image_view,
        m_depth_image_view,
    };

    VkFramebufferCreateInfo const fb_info{
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0U,
        .renderPass      = m_render_pass,
        .attachmentCount = VK_SIZE_CAST(attachment_views.size()),
        .pAttachments    = attachment_views.data(),
        .width           = m_swapchain->GetExtent().width,
        .height          = m_swapchain->GetExtent().height,
        .layers          = 1U,
    };

    VkFramebuffer framebuffer{VK_NULL_HANDLE};
    assert(vkCreateFramebuffer(m_device, &fb_info, nullptr, &framebuffer) == VK_SUCCESS);

    return framebuffer;
}

VkPipeline GeneralPass::createPipeline()
{
    std::array<VkDynamicState, 2> dynamic_states{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    std::array<Shader const*, 2> shaders{
        m_shader_cache->GetShaderModule("s_basiccolor_vt"_sid),
        m_shader_cache->GetShaderModule("s_basiccolor_fm"_sid),
    };

    std::array<VkPipelineColorBlendAttachmentState, 1> color_blend_attachments{
        VkPipelineColorBlendAttachmentState{
            .blendEnable         = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp        = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp        = VK_BLEND_OP_ADD,
            .colorWriteMask      = static_cast<VkColorComponentFlags>(VK_COLOR_COMPONENT_R_BIT) |
                                   static_cast<VkColorComponentFlags>(VK_COLOR_COMPONENT_G_BIT) |
                                   static_cast<VkColorComponentFlags>(VK_COLOR_COMPONENT_B_BIT) |
                                   static_cast<VkColorComponentFlags>(VK_COLOR_COMPONENT_A_BIT),
        },
    };

    auto result = m_pipeline_cache->ConstructPipeline(GraphicsPipelineInfo{
        .name               = "pl_basiccolor"_sid,
        .shaders            = shaders,
        .topology           = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .vertex_input       = CreateVertexDescription(VertexAttributes::kWithPosition | VertexAttributes::kWithColors |
                                                      VertexAttributes::kWithTexCoords),
        .polygon_mode       = VK_POLYGON_MODE_FILL,
        .cull_mode          = VK_CULL_MODE_BACK_BIT,
        .front_face         = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depth_bias_enable  = VK_FALSE,
        .multisampling      = VK_SAMPLE_COUNT_1_BIT,
        .depth_test_enable  = VK_TRUE,
        .depth_write_enable = VK_TRUE,
        .depth_compare_op   = VK_COMPARE_OP_LESS,
        .color_blend_attachments       = color_blend_attachments,
        .dynamic_states                = dynamic_states,
        .render_pass                   = m_render_pass,
        .subpass                       = 0U,
        .descriptor_set_layout_binding = {{
            VkDescriptorSetLayoutBinding{
                .binding            = 0U,
                .descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount    = 1U,
                .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr,
            },
            VkDescriptorSetLayoutBinding{
                .binding            = 1U,
                .descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount    = 1U,
                .stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            },
            VkDescriptorSetLayoutBinding{
                .binding            = 2U,
                .descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount    = 1U,
                .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr,
            },
        }}});

    assert(result.has_value() && "Failed to create basic color pipeline!");

    return result->pipeline;
}

VkBuffer GeneralPass::createVertexBuffer()
{
    VkDeviceSize const buffer_size = sizeof(Vertex) * m_vertices.size();

    auto buf = m_vrm->CreateBuffer(
        VkBufferCreateInfo{
            .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext                 = nullptr,
            .flags                 = 0U,
            .size                  = buffer_size,
            .usage                 = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0U,
            .pQueueFamilyIndices   = nullptr,
        },
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_HOST);

    assert(buf.has_value());

    m_buffer_vertices_index = *buf;

    vmaCopyMemoryToAllocation(m_context.GetVulkanDevice().allocator,
                              m_vertices.data(),
                              m_vrm->GetBuffer(m_buffer_vertices_index)->allocation,
                              0U,
                              buffer_size);

    return m_vrm->GetBuffer(m_buffer_vertices_index)->buffer;
}

VkBuffer GeneralPass::createIndexBuffer()
{
    VkDeviceSize const buffer_size = sizeof(std::uint16_t) * m_indices.size();

    auto buf = m_vrm->CreateBuffer(
        VkBufferCreateInfo{
            .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext                 = nullptr,
            .flags                 = 0U,
            .size                  = buffer_size,
            .usage                 = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0U,
            .pQueueFamilyIndices   = nullptr,
        },
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_HOST);

    assert(buf.has_value());

    m_buffer_indices_index = *buf;

    vmaCopyMemoryToAllocation(m_context.GetVulkanDevice().allocator,
                              m_indices.data(),
                              m_vrm->GetBuffer(m_buffer_indices_index)->allocation,
                              0U,
                              buffer_size);

    return m_vrm->GetBuffer(m_buffer_indices_index)->buffer;
}

void GeneralPass::createUboMapping()
{
    for (std::size_t i{0U}; i < m_context.GetFramesInFlight(); ++i)
    {
        // UBO
        {
            auto buf = m_vrm->CreateBuffer(
                VkBufferCreateInfo{
                    .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                    .pNext                 = nullptr,
                    .flags                 = 0U,
                    .size                  = sizeof(UniformBufferObject),
                    .usage                 = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
                    .queueFamilyIndexCount = 0U,
                    .pQueueFamilyIndices   = nullptr,
                },
                VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
                VMA_MEMORY_USAGE_AUTO_PREFER_HOST);

            assert(buf.has_value());

            m_buffer_indices_ubos[i] = *buf;

            vmaMapMemory(m_context.GetVulkanDevice().allocator,
                         m_vrm->GetBuffer(m_buffer_indices_ubos[i])->allocation,
                         &m_ubo_mappings[i]);
        }

        // SSBO
        {
            VkDeviceSize const buffer_size = sizeof(QuadInstance) * kMaxQuadInstances;
            auto               buf         = m_vrm->CreateBuffer(
                VkBufferCreateInfo{
                    .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                    .pNext                 = nullptr,
                    .flags                 = 0U,
                    .size                  = buffer_size,
                    .usage                 = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                    .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
                    .queueFamilyIndexCount = 0U,
                    .pQueueFamilyIndices   = nullptr,
                },
                VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
                VMA_MEMORY_USAGE_AUTO_PREFER_HOST);

            assert(buf.has_value());

            m_buffer_instancing_indices[i] = *buf;

            vmaMapMemory(m_context.GetVulkanDevice().allocator,
                         m_vrm->GetBuffer(m_buffer_instancing_indices[i])->allocation,
                         &m_instance_mappings[i]);
        }
    }
}

VkDescriptorPool GeneralPass::createDescriptorPool()
{
    std::size_t const                   frames_in_flight = m_context.GetFramesInFlight();
    std::array<VkDescriptorPoolSize, 3> pool_sizes{
        VkDescriptorPoolSize{
            .type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = VK_SIZE_CAST(frames_in_flight),
        },
        VkDescriptorPoolSize{
            .type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = VK_SIZE_CAST(frames_in_flight),
        },
        VkDescriptorPoolSize{
            .type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = VK_SIZE_CAST(frames_in_flight),
        },
    };

    VkDescriptorPoolCreateInfo const pool_info{
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0U,
        .maxSets       = VK_SIZE_CAST(frames_in_flight),
        .poolSizeCount = VK_SIZE_CAST(pool_sizes.size()),
        .pPoolSizes    = pool_sizes.data(),
    };

    VkDescriptorPool descriptor_pool{VK_NULL_HANDLE};
    assert(vkCreateDescriptorPool(m_device, &pool_info, nullptr, &descriptor_pool) == VK_SUCCESS);

    return descriptor_pool;
}

void GeneralPass::createDescriptorSets()
{
    std::size_t const frames_in_flight = m_context.GetFramesInFlight();

    std::vector<VkDescriptorSetLayout> layouts(
        frames_in_flight,
        m_pipeline_cache->GetPipeline("pl_basiccolor"_sid)->descriptor_set_layouts[0U]);

    VkDescriptorSetAllocateInfo const alloc_info{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = m_descriptor_pool,
        .descriptorSetCount = VK_SIZE_CAST(frames_in_flight),
        .pSetLayouts        = layouts.data(),
    };

    assert(vkAllocateDescriptorSets(m_device, &alloc_info, m_descriptor_sets.data()) == VK_SUCCESS);

    for (size_t i = 0; i < frames_in_flight; i++)
    {
        VkDescriptorBufferInfo buffer_info{
            .buffer = m_vrm->GetBuffer(m_buffer_indices_ubos[i])->buffer,
            .offset = 0U,
            .range  = VK_WHOLE_SIZE,
        };

        VkDescriptorBufferInfo ssbo_info{
            .buffer = m_vrm->GetBuffer(m_buffer_instancing_indices[i])->buffer,
            .offset = 0U,
            .range  = VK_WHOLE_SIZE,
        };

        VkDescriptorImageInfo image_info{
            .sampler     = m_sampler_tux_texture,
            .imageView   = m_image_view_tux_texture,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        std::array<VkWriteDescriptorSet, 3> descriptor_writes{
            VkWriteDescriptorSet{
                .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext            = nullptr,
                .dstSet           = m_descriptor_sets[i],
                .dstBinding       = 0U,
                .dstArrayElement  = 0U,
                .descriptorCount  = 1U,
                .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pImageInfo       = nullptr,
                .pBufferInfo      = &buffer_info,
                .pTexelBufferView = nullptr,
            },
            VkWriteDescriptorSet{
                .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext            = nullptr,
                .dstSet           = m_descriptor_sets[i],
                .dstBinding       = 1U,
                .dstArrayElement  = 0U,
                .descriptorCount  = 1U,
                .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo       = &image_info,
                .pBufferInfo      = nullptr,
                .pTexelBufferView = nullptr,
            },
            VkWriteDescriptorSet{
                .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext            = nullptr,
                .dstSet           = m_descriptor_sets[i],
                .dstBinding       = 2U,
                .dstArrayElement  = 0U,
                .descriptorCount  = 1U,
                .descriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pImageInfo       = nullptr,
                .pBufferInfo      = &ssbo_info,
                .pTexelBufferView = nullptr,
            },
        };

        vkUpdateDescriptorSets(m_device, VK_SIZE_CAST(descriptor_writes.size()), descriptor_writes.data(), 0U, nullptr);
    }
}

VkImageView GeneralPass::createTexture()
{
    auto img_res = m_vrm->LoadTextureToImage("Resource/Textures/tux.png",
                                             VK_FORMAT_R8G8B8A8_SRGB,
                                             VK_IMAGE_USAGE_SAMPLED_BIT,
                                             VK_IMAGE_LAYOUT_UNDEFINED,
                                             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

    m_texture_image_index = *img_res;

    return util::CreateImageView(m_device,
                                 m_vrm->GetImage(m_texture_image_index)->image,
                                 VK_FORMAT_R8G8B8A8_SRGB,
                                 VK_IMAGE_ASPECT_COLOR_BIT);
}

VkSampler GeneralPass::createTextureSampler()
{
    VkPhysicalDeviceProperties phys_dev_props{};
    vkGetPhysicalDeviceProperties(m_context.GetVulkanDevice().phys_device, &phys_dev_props);

    VkSamplerCreateInfo const sampler_info{
        .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .magFilter               = VK_FILTER_LINEAR,
        .minFilter               = VK_FILTER_LINEAR,
        .mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias              = 0.0F,
        .anisotropyEnable        = VK_TRUE,
        .maxAnisotropy           = phys_dev_props.limits.maxSamplerAnisotropy,
        .compareEnable           = VK_FALSE,
        .compareOp               = VK_COMPARE_OP_ALWAYS,
        .minLod                  = 0.0F,
        .maxLod                  = VK_LOD_CLAMP_NONE,
        .borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };

    VkSampler sampler{VK_NULL_HANDLE};
    assert(VK_SUCCESS == vkCreateSampler(m_device, &sampler_info, nullptr, &sampler));

    return sampler;
}

void GeneralPass::createDepthResources()
{
    auto depth_image_res = m_vrm->CreateImage(
        VkImageCreateInfo{
            .sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext     = nullptr,
            .flags     = 0U,
            .imageType = VK_IMAGE_TYPE_2D,
            .format    = VK_FORMAT_D32_SFLOAT,
            .extent =
                {
                    .width  = m_swapchain->GetExtent().width,
                    .height = m_swapchain->GetExtent().height,
                    .depth  = 1U,
                },
            .mipLevels             = 1U,
            .arrayLayers           = 1U,
            .samples               = VK_SAMPLE_COUNT_1_BIT,
            .tiling                = VK_IMAGE_TILING_OPTIMAL,
            .usage                 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0U,
            .pQueueFamilyIndices   = nullptr,
            .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
        },
        0U,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    assert(depth_image_res.has_value());

    m_depth_image_index = *depth_image_res;
    m_vrm->GetImage(m_depth_image_index);

    m_depth_image_view = util::CreateImageView(m_device,
                                               m_vrm->GetImage(m_depth_image_index)->image,
                                               VK_FORMAT_D32_SFLOAT,
                                               VK_IMAGE_ASPECT_DEPTH_BIT);
}

}// namespace polos::rendering
