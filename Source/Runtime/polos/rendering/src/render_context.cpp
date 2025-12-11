//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/render_context.hpp"

#include "polos/communication/event_bus.hpp"
#include "polos/communication/window_framebuffer_resize.hpp"
#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/allocated_image.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/pipeline_cache.hpp"
#include "polos/rendering/render_pass_layout_description.hpp"
#include "polos/rendering/rendering_api.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/shader_cache.hpp"
#include "polos/rendering/vulkan_context.hpp"
#include "polos/rendering/vulkan_device.hpp"
#include "polos/rendering/vulkan_resource_manager.hpp"
#include "polos/rendering/vulkan_swapchain.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>

#define INIT_VULKAN_COMPONENT(SubmodulePtr, ...)         \
    {                                                    \
        auto result = SubmodulePtr->Create(__VA_ARGS__); \
        if (!result.has_value())                         \
        {                                                \
            return ErrorType{result.error()};            \
        }                                                \
    }

namespace polos::rendering
{
namespace
{

auto createVertexBuffer = [](VmaAllocator           t_allocator,
                             allocated_buffer*&     t_buf,
                             std::span<Vertex>      t_vertices,
                             VulkanResourceManager* t_vrm) -> void {
    VkDeviceSize const buffer_size = sizeof(Vertex) * t_vertices.size();

    auto buf = t_vrm->CreateBuffer(
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

    t_buf = *buf;

    vmaCopyMemoryToAllocation(t_allocator, t_vertices.data(), t_buf->allocation, 0U, buffer_size);
};

auto createIndexBuffer = [](VmaAllocator             t_allocator,
                            allocated_buffer*&       t_buf,
                            std::span<std::uint16_t> t_indices,
                            VulkanResourceManager*   t_vrm) -> void {
    VkDeviceSize const buffer_size = sizeof(std::uint16_t) * t_indices.size();

    auto buf = t_vrm->CreateBuffer(
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

    t_buf = *buf;

    vmaCopyMemoryToAllocation(t_allocator, t_indices.data(), t_buf->allocation, 0U, buffer_size);
};

auto FindQueueFamily(VkPhysicalDevice t_device, VkSurfaceKHR t_surface, VkQueueFlags t_flags) -> Result<std::uint32_t>
{
    std::uint32_t queue_family_count{0U};
    vkGetPhysicalDeviceQueueFamilyProperties(t_device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families{queue_family_count};
    vkGetPhysicalDeviceQueueFamilyProperties(t_device, &queue_family_count, queue_families.data());

    std::uint32_t queue_index{0U};
    auto const    queue_families_size = static_cast<std::uint32_t>(queue_families.size());
    for (; queue_index < queue_families_size; ++queue_index)
    {
        VkQueueFamilyProperties const& family = queue_families[queue_index];

        if (0U != (family.queueFlags & t_flags))
        {
            if (0U != (t_flags & VK_QUEUE_GRAPHICS_BIT))
            {
                // If gfx family is requested, also check if it has present capabilities
                VkBool32 present_support{0U};
                vkGetPhysicalDeviceSurfaceSupportKHR(t_device, queue_index, t_surface, &present_support);

                if (0U != present_support)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    if (queue_index == VK_SIZE_CAST(queue_families.size()))
    {
        return ErrorType{RenderingErrc::kNoAdequateQueueFamily};
    }

    return queue_index;
}

namespace basic_color_pipeline
{

constexpr VkAttachmentReference2 const kColorAttachmentRef{
    .sType      = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
    .pNext      = nullptr,
    .attachment = 0U,
    .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
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
    .pDepthStencilAttachment = nullptr,
    .preserveAttachmentCount = 0U,
    .pPreserveAttachments    = nullptr,
};

constexpr VkSubpassDependency2 const kSubpassDependency{
    .sType           = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
    .pNext           = nullptr,
    .srcSubpass      = VK_SUBPASS_EXTERNAL,
    .dstSubpass      = 0U,
    .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    .srcAccessMask   = 0U,
    .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    .dependencyFlags = 0U,
    .viewOffset      = 0U,
};

}// namespace basic_color_pipeline

}// namespace

RenderContext::RenderContext()
    : m_image_acq_results{{ImageAcqusitionResult::kSuccess}}
{
    communication::Subscribe<communication::window_framebuffer_resize>(
        [this](communication::window_framebuffer_resize&) {
            m_framebuffer_resized = true;
            LogInfo("Framebuffer resize event received in RenderContext.");
        });
};

RenderContext::~RenderContext() = default;

auto RenderContext::Initialize(GLFWwindow* t_window) -> Result<void>
{
    m_window         = t_window;
    m_context        = std::make_unique<VulkanContext>();
    m_device         = std::make_unique<VulkanDevice>();
    m_swapchain      = std::make_unique<VulkanSwapchain>(m_window);
    m_vrm            = std::make_unique<VulkanResourceManager>();
    m_shader_cache   = std::make_unique<ShaderCache>();
    m_pipeline_cache = std::make_unique<PipelineCache>();

    m_vertices_basic_color = {
        // RGB Triangle
        Vertex{
            .position = {0.0F, -0.5F, 0.0F},
            .color    = {1.0F, 0.0F, 0.0F},
        },
        Vertex{
            .position = {0.5F, 0.5F, 0.0F},
            .color    = {0.0F, 1.0F, 0.0F},
        },
        Vertex{
            .position = {-0.5F, 0.5F, 0.0F},
            .color    = {0.0F, 0.0F, 1.0F},
        },
    };

    m_indices_basic_color = {
        0U,
        1U,
        2U,
    };

    for (auto& vert : m_vertices_basic_color) { vert.position.x -= 0.25F; }

    // --- Initialize Vulkan context ---
    INIT_VULKAN_COMPONENT(m_context);

    // --- Surface Creation ---
    CHECK_VK_SUCCESS_OR_ERR(
        glfwCreateWindowSurface(m_context->instance, m_window, nullptr, &m_surface),
        RenderingErrc::kFailedCreateSurface);

    // --- Physical device selection ---
    std::uint32_t device_count{0U};
    vkEnumeratePhysicalDevices(m_context->instance, &device_count, nullptr);
    if (0U == device_count)
    {
        return ErrorType{RenderingErrc::kFailedFindPhysDevice};
    }

    std::vector<VkPhysicalDevice> physical_devices(static_cast<std::size_t>(device_count));
    vkEnumeratePhysicalDevices(m_context->instance, &device_count, physical_devices.data());

    // TODO(sorbatdev): Select the first available GPU, no need for suitability checks for now.
    VkPhysicalDevice phys_device = physical_devices[0];

    // --- Suitable queue family selection ---
    {// Graphics Family
        auto const q_result = FindQueueFamily(phys_device, m_surface, VK_QUEUE_GRAPHICS_BIT);
        if (!q_result.has_value())
        {
            return ErrorType{RenderingErrc::kNoAdequateQueueFamily};
        }
        m_queue_family_indices.gfx_q_index = q_result.value();
    }
    {// Transfer Family
        auto const q_result = FindQueueFamily(phys_device, m_surface, VK_QUEUE_TRANSFER_BIT);
        if (!q_result.has_value())
        {
            return ErrorType{RenderingErrc::kNoAdequateQueueFamily};
        }
        m_queue_family_indices.transfer_q_index = q_result.value();
    }
    {// Compute Family
        auto const q_result = FindQueueFamily(phys_device, m_surface, VK_QUEUE_COMPUTE_BIT);
        if (!q_result.has_value())
        {
            return ErrorType{RenderingErrc::kNoAdequateQueueFamily};
        }

        m_queue_family_indices.compute_q_index = q_result.value();
    }

    // --- Device creation ---
    {
        device_create_details const info{
            .instance           = m_context->instance,
            .surface            = m_surface,
            .phys_device        = phys_device,
            .q_indices          = m_queue_family_indices,
            .enabled_extensions = {{VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME}},
        };
        INIT_VULKAN_COMPONENT(m_device, info);
    }

    // Get the graphics queue for now
    vkGetDeviceQueue(m_device->logi_device, m_queue_family_indices.gfx_q_index, 0U, &m_gfx_queue);

    // --- Swapchain creation ---
    {
        swapchain_create_details const details{
            .device      = m_device.get(),
            .phys_device = phys_device,
            .surface     = m_surface,
            .preferred_surface_formats =
                {
                    {
                        .format     = VK_FORMAT_B8G8R8A8_SRGB,
                        .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                    },
                    {
                        .format     = VK_FORMAT_R8G8B8A8_SRGB,
                        .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                    },
                },
            .preferred_present_modes =
                {
                    VK_PRESENT_MODE_MAILBOX_KHR,
                    VK_PRESENT_MODE_FIFO_KHR,
                },
            .gfx_queue       = m_gfx_queue,
            .transform_flags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        };

        INIT_VULKAN_COMPONENT(m_swapchain, details);


        VkFormat const      sc_img_fmt = m_swapchain->GetSurfaceFormat().format;
        std::uint32_t const img_count  = m_swapchain->GetImageCount();
        m_swapchain_images.resize(img_count);

        for (std::uint32_t i{0U}; i < img_count; ++i)
        {
            m_swapchain_images[i].image      = m_swapchain->GetImage(i);
            m_swapchain_images[i].image_view = m_swapchain->GetImageView(i);
            m_swapchain_images[i].format     = sc_img_fmt;
            m_swapchain_images[i].samples    = VK_SAMPLE_COUNT_1_BIT;
        }
    }

    {
        resource_manager_create_details const details{
            .device    = m_device->logi_device,
            .allocator = m_device->allocator,
            .swapchain = m_swapchain.get(),
        };

        INIT_VULKAN_COMPONENT(m_vrm, details);
    }

    {
        pipeline_cache_create_details const details{
            .logi_device = m_device->logi_device,
            .swapchain   = m_swapchain.get(),
        };

        INIT_VULKAN_COMPONENT(m_pipeline_cache, details);
    }

    {
        shader_cache_create_details const details{
            .logi_device = m_device->logi_device,
            .shader_files =
                {
                    {
                        .custom_name = "s_basiccolor_vt",
                        .stage       = ShaderStage::kVertex,
                        .path        = "Resource/Shaders/basic_color.vert.spv",
                    },
                    {
                        .custom_name = "s_basiccolor_fm",
                        .stage       = ShaderStage::kFragment,
                        .path        = "Resource/Shaders/basic_color.frag.spv",
                    },
                },
        };

        INIT_VULKAN_COMPONENT(m_shader_cache, details);
    }

    // --- Command Pool creation ---
    VkCommandPoolCreateInfo const pool_info{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = m_queue_family_indices.gfx_q_index,
    };

    CHECK_VK_SUCCESS_OR_ERR(
        vkCreateCommandPool(m_device->logi_device, &pool_info, nullptr, &m_command_pool),
        RenderingErrc::kFailedCreateCmdPool);

    m_frame_command_buffers.resize(kMaxFramesInFlight);
    m_acquire_semaphores.resize(kMaxFramesInFlight);

    // Create submission semaphores with number of images in swapchain
    // https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html#_discussion_of_solution
    m_submit_semaphores.resize(m_swapchain->GetImageCount());
    m_frame_fences.resize(kMaxFramesInFlight);

    // --- Command buffer allocations ---
    VkCommandBufferAllocateInfo const alloc_info{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = m_command_pool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = kMaxFramesInFlight,
    };

    CHECK_VK_SUCCESS_OR_ERR(
        vkAllocateCommandBuffers(m_device->logi_device, &alloc_info, m_frame_command_buffers.data()),
        RenderingErrc::kFailedCmdBufAlloc);

    // --- Synchronization objects creation ---
    VkSemaphoreCreateInfo const semaphore_info{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
    };
    VkFenceCreateInfo const fence_info{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for (std::uint32_t i{0U}; i < m_swapchain->GetImageCount(); ++i)
    {
        CHECK_VK_SUCCESS_OR_ERR(
            vkCreateSemaphore(m_device->logi_device, &semaphore_info, nullptr, &m_submit_semaphores[i]),
            RenderingErrc::kFailedCreateSemaphore);
    }

    for (std::size_t i{0U}; i < kMaxFramesInFlight; ++i)
    {
        CHECK_VK_SUCCESS_OR_ERR(
            vkCreateSemaphore(m_device->logi_device, &semaphore_info, nullptr, &m_acquire_semaphores[i]),
            RenderingErrc::kFailedCreateSemaphore);
        CHECK_VK_SUCCESS_OR_ERR(
            vkCreateFence(m_device->logi_device, &fence_info, nullptr, &m_frame_fences[i]),
            RenderingErrc::kFailedCreateFence);
    }

    // --- Basic Color Pipeline Creation ---
    {
        {
            auto result = CreateRenderPass(
                render_pass_layout_description{
                    .attachments =
                        {
                            VkAttachmentDescription2{
                                .sType          = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
                                .pNext          = nullptr,
                                .flags          = 0U,
                                .format         = m_swapchain->GetSurfaceFormat().format,
                                .samples        = VK_SAMPLE_COUNT_1_BIT,
                                .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
                                .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
                                .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                            },
                        },
                    .subpasses    = {basic_color_pipeline::kSubpassDesc},
                    .dependencies = {basic_color_pipeline::kSubpassDependency},
                });
            if (!result.has_value())
            {
                LogError("Could not create compatible VkRenderPass.");
                return ErrorType{result.error()};
            }

            m_vk_render_passes[0U] = *result;
        }

        std::array<shader const*, 2> shaders{
            m_shader_cache->GetShaderModule("s_basiccolor_vt"_sid),
            m_shader_cache->GetShaderModule("s_basiccolor_fm"_sid),
        };

        std::array<VkPipelineColorBlendAttachmentState, 1> color_blend_attachments{
            VkPipelineColorBlendAttachmentState{
                .blendEnable         = VK_FALSE,
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

        std::array<VkDynamicState, 2> dynamic_states{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        auto result = m_pipeline_cache->ConstructPipeline(
            graphics_pipeline_info{
                .name     = "pl_basiccolor"_sid,
                .shaders  = shaders,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .vertex_input =
                    CreateVertexDescription(VertexAttributes::kWithPosition | VertexAttributes::kWithColors),
                .polygon_mode            = VK_POLYGON_MODE_FILL,
                .cull_mode               = VK_CULL_MODE_BACK_BIT,
                .front_face              = VK_FRONT_FACE_CLOCKWISE,
                .depth_bias_enable       = VK_FALSE,
                .multisampling           = VK_SAMPLE_COUNT_1_BIT,
                .depth_test_enable       = VK_TRUE,
                .depth_write_enable      = VK_TRUE,
                .depth_compare_op        = VK_COMPARE_OP_GREATER,
                .color_blend_attachments = color_blend_attachments,
                .dynamic_states          = dynamic_states,
                .render_pass             = m_vk_render_passes[0U],
                .subpass                 = 0U,
            });
        if (!result.has_value())
        {
            LogError("Could not create BasicColorPipeline");
            return ErrorType{result.error()};
        }

        m_pipeline_basic_color = result->pipeline;

        createVertexBuffer(m_device->allocator, m_buf_basic_color_vert, m_vertices_basic_color, m_vrm.get());
        createIndexBuffer(m_device->allocator, m_buf_basic_color_idx, m_indices_basic_color, m_vrm.get());
    }

    m_is_initialized = true;

    return {};
}

auto RenderContext::BeginFrame() -> VkCommandBuffer
{
    vkWaitForFences(
        m_device->logi_device,
        1U,
        &m_frame_fences[m_current_frame_index],
        VK_TRUE,
        std::numeric_limits<std::uint64_t>::max());


    if (!m_transient_fbufs[m_current_frame_index].empty())
    {
        for (auto* fbuf : m_transient_fbufs[m_current_frame_index])
        {
            vkDestroyFramebuffer(m_device->logi_device, fbuf, nullptr);
        }
        m_transient_fbufs[m_current_frame_index].clear();
    }

    acquire_next_image_details const next_img_dets{
        .semaphore = m_acquire_semaphores[m_current_frame_index],
        .fence     = m_frame_fences[m_current_frame_index],
        .timeout   = std::numeric_limits<std::uint64_t>::max(),
    };

    auto const acq_result = m_swapchain->AcquireNextImage(next_img_dets);
    if (!acq_result.has_value())
    {
        m_image_acq_results[m_current_frame_index] = ImageAcqusitionResult::kError;
        onFramebufferResize();
        LogError("{}", acq_result.error());
        return VK_NULL_HANDLE;
    }

    vkResetFences(m_device->logi_device, 1U, &m_frame_fences[m_current_frame_index]);

    m_swapchain_image_index = *acq_result;

    VkCommandBuffer                cur_cmd_buf = m_frame_command_buffers[m_current_frame_index];
    VkCommandBufferBeginInfo const begin_info{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = 0U,
        .pInheritanceInfo = nullptr,
    };

    vkResetCommandBuffer(cur_cmd_buf, 0U);
    if (vkBeginCommandBuffer(cur_cmd_buf, &begin_info) != VK_SUCCESS)
    {
        LogError("Could not begin recording command buffer!");
        return VK_NULL_HANDLE;
    }

    return cur_cmd_buf;
}

auto RenderContext::renderFrame() -> void
{
    std::span<render_object> const render_objects = RenderingApi::GetMainScene()->GetObjects();

    texture_description const& cur_frame_desc = m_swapchain_images[m_swapchain_image_index];
    VkCommandBuffer            cur_cmd_buf    = m_frame_command_buffers[m_current_frame_index];
    // GeneralGeometryPass
    {
        std::array<VkImageView, 1U> const attachment_views{
            cur_frame_desc.image_view,
        };

        VkFramebufferCreateInfo const fb_info{
            .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext           = nullptr,
            .flags           = 0U,
            .renderPass      = m_vk_render_passes[0U],
            .attachmentCount = 1U,
            .pAttachments    = attachment_views.data(),
            .width           = m_swapchain->GetExtent().width,
            .height          = m_swapchain->GetExtent().height,
            .layers          = 1U,
        };

        VkFramebuffer pass_fb{VK_NULL_HANDLE};
        vkCreateFramebuffer(m_device->logi_device, &fb_info, nullptr, &pass_fb);
        AddFramebufferToCurrentFrame(pass_fb);

        VkClearValue const clear_color{
            .color = {.float32 = {kPolosRed, kPolosGreen, kPolosBlue, 1.0F}},
        };

        VkRenderPassBeginInfo const pass_begin_info{
            .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext       = nullptr,
            .renderPass  = m_vk_render_passes[0U],
            .framebuffer = pass_fb,
            .renderArea =
                {
                    .offset = {0, 0},
                    .extent = m_swapchain->GetExtent(),
                },
            .clearValueCount = 1U,
            .pClearValues    = &clear_color,
        };

        // clang-format off

        vkCmdBeginRenderPass(cur_cmd_buf, &pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdSetViewport(cur_cmd_buf, 0U, 1U, &m_swapchain->GetViewport());
            vkCmdSetScissor(cur_cmd_buf, 0U, 1U, &m_swapchain->GetScissor());
            vkCmdBindPipeline(cur_cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_basic_color);

            // Bind vertex buffer
            VkDeviceSize const offset{0U};
            vkCmdBindVertexBuffers(cur_cmd_buf, 0U, 1U, &m_buf_basic_color_vert->buffer, &offset);
            vkCmdBindIndexBuffer(cur_cmd_buf, m_buf_basic_color_idx->buffer, offset, VK_INDEX_TYPE_UINT16);

            for (auto objects : render_objects) { vkCmdDrawIndexed(cur_cmd_buf, VK_SIZE_CAST(m_indices_basic_color.size()), 1U, 0U, 0U, 0U); }
        vkCmdEndRenderPass(cur_cmd_buf);

        // clang-format on
    }
}

auto RenderContext::EndFrame() -> void
{
    renderFrame();

    // If image acquisition failed, skip rendering and presentation for this frame
    if (m_image_acq_results[m_current_frame_index] == ImageAcqusitionResult::kError)
    {
        m_image_acq_results[m_current_frame_index] = ImageAcqusitionResult::kSuccess;
        return;
    }

    if (VK_SUCCESS != vkEndCommandBuffer(m_frame_command_buffers[m_current_frame_index]))
    {
        LogError("Could not record command buffer.");
        return;
    }

    std::array<VkPipelineStageFlags, 1> const wait_stages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo const submit_info{
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = 1U,
        .pWaitSemaphores      = &m_acquire_semaphores[m_current_frame_index],
        .pWaitDstStageMask    = wait_stages.data(),
        .commandBufferCount   = 1U,
        .pCommandBuffers      = &m_frame_command_buffers[m_current_frame_index],
        .signalSemaphoreCount = 1U,
        .pSignalSemaphores    = &m_submit_semaphores[m_swapchain_image_index],
    };

    if (VK_SUCCESS != vkQueueSubmit(m_gfx_queue, 1U, &submit_info, m_frame_fences[m_current_frame_index]))
    {
        LogError("Could not submit draw command buffer to the graphics queue!");
        return;
    }

    if (auto const result = m_swapchain->QueuePresent(m_submit_semaphores[m_swapchain_image_index]);
        !result.has_value())
    {
        if (m_framebuffer_resized)
        {
            m_framebuffer_resized = false;
        }
        onFramebufferResize();
        LogError("{}", result.error());
    }

    m_current_frame_index = (m_current_frame_index + 1) % kMaxFramesInFlight;

    // Handle framebuffer resize anyways in case it was not caught during presentation
    if (m_framebuffer_resized)
    {
        m_framebuffer_resized = false;
        onFramebufferResize();
    }
}

auto RenderContext::GetShaderCache() const -> ShaderCache&
{
    return *m_shader_cache;
}

auto RenderContext::GetPipelineCache() const -> PipelineCache&
{
    return *m_pipeline_cache;
}

auto RenderContext::IsInitialized() const -> bool
{
    return m_is_initialized;
}

auto RenderContext::GetSwapchain() -> VulkanSwapchain&
{
    return *m_swapchain;
}

auto RenderContext::CreateRenderPass(render_pass_layout_description const& t_layout) -> Result<VkRenderPass>
{
    VkRenderPassCreateInfo2 const pass_info{
        .sType                   = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .attachmentCount         = VK_SIZE_CAST(t_layout.attachments.size()),
        .pAttachments            = t_layout.attachments.data(),
        .subpassCount            = VK_SIZE_CAST(t_layout.subpasses.size()),
        .pSubpasses              = t_layout.subpasses.data(),
        .dependencyCount         = VK_SIZE_CAST(t_layout.dependencies.size()),
        .pDependencies           = t_layout.dependencies.data(),
        .correlatedViewMaskCount = 0U,
        .pCorrelatedViewMasks    = nullptr,
    };

    VkRenderPass vk_render_pass{VK_NULL_HANDLE};

    CHECK_VK_SUCCESS_OR_ERR(
        vkCreateRenderPass2(m_device->logi_device, &pass_info, nullptr, &vk_render_pass),
        RenderingErrc::kFailedCreateRenderPass);

    m_vk_render_passes.push_back(vk_render_pass);

    return vk_render_pass;
}

auto RenderContext::AddFramebufferToCurrentFrame(VkFramebuffer t_fbuf) -> void
{
    m_transient_fbufs[m_current_frame_index].push_back(t_fbuf);
}

void RenderContext::onFramebufferResize()
{
    vkDeviceWaitIdle(m_device->logi_device);
    LogInfo("Recreating swapchain due to framebuffer resize...");

    std::ignore = m_swapchain->Destroy();
    swapchain_create_details const details{
        .device      = m_device.get(),
        .phys_device = m_device->phys_device,
        .surface     = m_surface,
        .preferred_surface_formats =
            {
                {
                    .format     = VK_FORMAT_B8G8R8A8_SRGB,
                    .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                },
                {
                    .format     = VK_FORMAT_R8G8B8A8_SRGB,
                    .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                },
            },
        .preferred_present_modes =
            {
                VK_PRESENT_MODE_MAILBOX_KHR,
                VK_PRESENT_MODE_FIFO_KHR,
            },
        .gfx_queue       = m_gfx_queue,
        .transform_flags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
    };
    vkDeviceWaitIdle(m_device->logi_device);

    if (auto const res = m_swapchain->Create(details); !res.has_value())
    {
        LogCritical("{}", res.error());
    }

    VkFormat const      sc_img_fmt = m_swapchain->GetSurfaceFormat().format;
    std::uint32_t const img_count  = m_swapchain->GetImageCount();

    for (std::uint32_t i{0U}; i < img_count; ++i)
    {
        m_swapchain_images[i].image      = m_swapchain->GetImage(i);
        m_swapchain_images[i].image_view = m_swapchain->GetImageView(i);
        m_swapchain_images[i].format     = sc_img_fmt;
    }
}

auto RenderContext::Shutdown() -> Result<void>
{
    if (!m_is_initialized)
    {
        return {};
    }

    LogInfo("Waiting for device idle to destroy Vulkan RenderContext...");

    vkDeviceWaitIdle(m_device->logi_device);

    LogInfo("Destroying Vulkan resources...");

    for (std::uint32_t i{0U}; i < m_swapchain->GetImageCount(); ++i)
    {
        vkDestroySemaphore(m_device->logi_device, m_submit_semaphores[i], nullptr);
    }

    for (std::size_t i{0U}; i < kMaxFramesInFlight; ++i)
    {
        vkDestroySemaphore(m_device->logi_device, m_acquire_semaphores[i], nullptr);
        vkDestroyFence(m_device->logi_device, m_frame_fences[i], nullptr);
    }
    vkDestroyCommandPool(m_device->logi_device, m_command_pool, nullptr);

    m_vrm->DestroyBuffer(m_buf_basic_color_vert->id);
    m_vrm->DestroyBuffer(m_buf_basic_color_idx->id);

    for (auto const& fbs : m_transient_fbufs)
    {
        std::ranges::for_each(fbs, [this](VkFramebuffer t_fbuf) {
            vkDestroyFramebuffer(m_device->logi_device, t_fbuf, nullptr);
        });
    }
    std::ignore = m_pipeline_cache->Destroy();
    for (auto* pass : m_vk_render_passes) { vkDestroyRenderPass(m_device->logi_device, pass, nullptr); }
    std::ignore = m_shader_cache->Destroy();
    std::ignore = m_vrm->Destroy();
    std::ignore = m_swapchain->Destroy();
    std::ignore = m_device->Destroy();
    vkDestroySurfaceKHR(m_context->instance, m_surface, nullptr);
    std::ignore = m_context->Destroy();

    LogInfo("Vulkan destroy complete!");

    return {};
}

}// namespace polos::rendering

polos::rendering::IRenderContext* CreateRenderContext()
{
    // Capsulated in PlatformManager
    return new polos::rendering::RenderContext{};// NOLINT
}
