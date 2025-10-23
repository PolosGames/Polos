//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/render_context.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/i_render_system.hpp"
#include "polos/rendering/pipeline_cache.hpp"
#include "polos/rendering/render_graph.hpp"
#include "polos/rendering/render_pass_layout_description.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/vulkan_context.hpp"
#include "polos/rendering/vulkan_device.hpp"
#include "polos/rendering/vulkan_resource_manager.hpp"
#include "polos/rendering/vulkan_swapchain.hpp"

#include "polos/rendering/render_pass/clear_screen_pass.hpp"
#include "polos/rendering/system/clear_screen_system.hpp"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <filesystem>

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
auto FindQueueFamily(VkPhysicalDevice t_device, VkSurfaceKHR t_surface, VkQueueFlags t_flags) -> Result<std::uint32_t>
{
    std::uint32_t queue_family_count{0U};
    vkGetPhysicalDeviceQueueFamilyProperties(t_device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families{queue_family_count};
    vkGetPhysicalDeviceQueueFamilyProperties(t_device, &queue_family_count, queue_families.data());

    std::uint32_t queue_index{0U};
    for (; queue_index < queue_families.size(); ++queue_index)
    {
        VkQueueFamilyProperties const& family = queue_families[queue_index];

        if (family.queueFlags & t_flags)
        {
            if (t_flags & VK_QUEUE_GRAPHICS_BIT)
            {
                // If gfx family is requested, also check if it has present capabilities
                VkBool32 present_support{false};
                vkGetPhysicalDeviceSurfaceSupportKHR(t_device, queue_index, t_surface, &present_support);

                if (present_support)
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
        return ErrorType{RenderingErrc::kPreferableQueueFamilyNotFound};
    }

    return queue_index;
}

}// namespace

RenderContext::RenderContext()  = default;
RenderContext::~RenderContext() = default;

auto RenderContext::Initialize(GLFWwindow* t_window) -> Result<void>
{
    m_window         = t_window;
    m_context        = std::make_unique<VulkanContext>();
    m_device         = std::make_unique<VulkanDevice>(m_window);
    m_swapchain      = std::make_unique<VulkanSwapchain>(m_window);
    m_vrm            = std::make_unique<VRM>();
    m_pipeline_cache = std::make_unique<PipelineCache>();
    m_render_graph   = std::make_unique<RenderGraph>();

    // --- Initialize Vulkan context ---
    INIT_VULKAN_COMPONENT(m_context);

    // --- Surface Creation ---
    CHECK_VK_SUCCESS_OR_ERR(
        glfwCreateWindowSurface(m_context->instance, m_window, nullptr, &m_surface),
        RenderingErrc::kSurfaceNotCreated);

    // --- Physical device selection ---
    std::uint32_t device_count{0U};
    vkEnumeratePhysicalDevices(m_context->instance, &device_count, nullptr);
    if (0U == device_count)
    {
        return ErrorType{RenderingErrc::kNoPhysicalDeviceOnHost};
    }

    std::vector<VkPhysicalDevice> physical_devices(static_cast<std::size_t>(device_count));
    vkEnumeratePhysicalDevices(m_context->instance, &device_count, physical_devices.data());

    // TODO: Select the first available GPU, no need for suitability checks for now.
    VkPhysicalDevice phys_device = physical_devices[0];

    // --- Suitable queue family selection ---
    {// Graphics Family
        auto q_result = FindQueueFamily(phys_device, m_surface, VK_QUEUE_GRAPHICS_BIT);
        if (!q_result.has_value())
        {
            return ErrorType{RenderingErrc::kPreferableQueueFamilyNotFound};
        }
        m_queue_family_indices.gfx_q_index = q_result.value();
    }
    {// Transfer Family
        auto q_result = FindQueueFamily(phys_device, m_surface, VK_QUEUE_TRANSFER_BIT);
        if (!q_result.has_value())
        {
            return ErrorType{RenderingErrc::kPreferableQueueFamilyNotFound};
        }
        m_queue_family_indices.transfer_q_index = q_result.value();
    }
    {// Compute Family
        auto q_result = FindQueueFamily(phys_device, m_surface, VK_QUEUE_COMPUTE_BIT);
        if (!q_result.has_value())
        {
            return ErrorType{RenderingErrc::kPreferableQueueFamilyNotFound};
        }
        m_queue_family_indices.compute_q_index = q_result.value();
    }

    // --- Device creation ---
    {
        device_create_details info{
            .instance           = m_context->instance,
            .surface            = m_surface,
            .phys_device        = phys_device,
            .q_indices          = m_queue_family_indices,
            .enabled_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
        };
        INIT_VULKAN_COMPONENT(m_device, info);
    }

    // Get the graphics queue for now
    vkGetDeviceQueue(m_device->logi_device, m_queue_family_indices.gfx_q_index, 0U, &m_gfx_queue);

    // --- Swapchain creation ---
    {
        swapchain_create_details details{
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
    }

    {
        resource_manager_create_details details{
            .device    = m_device->logi_device,
            .swapchain = m_swapchain.get(),
            .shader_files =
                {
                    {"Basic Color", std::filesystem::path{"Resource/Shaders/basic_color.slang.spv"}},
                },
        };

        INIT_VULKAN_COMPONENT(m_vrm, details);
    }

    // --- Command Pool creation ---
    VkCommandPoolCreateInfo pool_info{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = m_queue_family_indices.gfx_q_index,
    };

    CHECK_VK_SUCCESS_OR_ERR(
        vkCreateCommandPool(m_device->logi_device, &pool_info, nullptr, &m_command_pool),
        RenderingErrc::kCommandPoolNotCreated);

    m_frame_command_buffers.resize(kMaxFramesInFlight);
    m_acquire_semaphores.resize(kMaxFramesInFlight);

    // Create submission semaphores with number of images in swapchain
    // https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html#_discussion_of_solution
    m_submit_semaphores.resize(m_swapchain->GetImageCount());
    m_frame_fences.resize(kMaxFramesInFlight);

    // --- Command buffer allocations ---
    VkCommandBufferAllocateInfo alloc_info{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = m_command_pool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = kMaxFramesInFlight,
    };

    CHECK_VK_SUCCESS_OR_ERR(
        vkAllocateCommandBuffers(m_device->logi_device, &alloc_info, m_frame_command_buffers.data()),
        RenderingErrc::kCommandBufferAllocationFail);

    // --- Synchronization objects creation ---
    VkSemaphoreCreateInfo semaphore_info{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
    };
    VkFenceCreateInfo fence_info{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for (std::uint32_t i{0U}; i < m_swapchain->GetImageCount(); ++i)
    {
        CHECK_VK_SUCCESS_OR_ERR(
            vkCreateSemaphore(m_device->logi_device, &semaphore_info, nullptr, &m_submit_semaphores[i]),
            RenderingErrc::kSemaphoreCreationFail);
    }

    for (std::size_t i{0U}; i < kMaxFramesInFlight; ++i)
    {
        CHECK_VK_SUCCESS_OR_ERR(
            vkCreateSemaphore(m_device->logi_device, &semaphore_info, nullptr, &m_acquire_semaphores[i]),
            RenderingErrc::kSemaphoreCreationFail);
        CHECK_VK_SUCCESS_OR_ERR(
            vkCreateFence(m_device->logi_device, &fence_info, nullptr, &m_frame_fences[i]),
            RenderingErrc::kFenceCreationFail);
    }

    {
        render_graph_creation_details details{
            .device  = m_device->logi_device,
            .context = this,
        };
        INIT_VULKAN_COMPONENT(m_render_graph, details);
    };

    m_render_systems.emplace_back(new ClearScreenSystem{*this, *m_render_graph});

    for (auto& system : m_render_systems) { system->Initialize(); }

    m_is_initialized = true;

    return {};
}

auto RenderContext::Shutdown() -> Result<void>
{
    if (!m_is_initialized)
        return {};

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

    for (auto const& fbs : m_transient_fbufs)
    {
        std::ranges::for_each(fbs, [this](VkFramebuffer t_fbuf) {
            vkDestroyFramebuffer(m_device->logi_device, t_fbuf, nullptr);
        });
    }
    std::ignore = m_pipeline_cache->Destroy();
    for (auto pass : m_vk_render_passes) { vkDestroyRenderPass(m_device->logi_device, pass, nullptr); }
    std::ignore = m_vrm->Destroy();

    std::ignore = m_render_graph->Destroy();
    std::ignore = m_swapchain->Destroy();
    std::ignore = m_device->Destroy();
    vkDestroySurfaceKHR(m_context->instance, m_surface, nullptr);
    std::ignore = m_context->Destroy();

    LogInfo("Vulkan destroy complete!");

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
        for (auto fb : m_transient_fbufs[m_current_frame_index])
        {
            vkDestroyFramebuffer(m_device->logi_device, fb, nullptr);
        }
        m_transient_fbufs[m_current_frame_index].clear();
    }
    m_render_graph->Reset();

    vkResetFences(m_device->logi_device, 1U, &m_frame_fences[m_current_frame_index]);

    acquire_next_image_details next_img_dets{
        .semaphore = m_acquire_semaphores[m_current_frame_index],
        .fence     = m_frame_fences[m_current_frame_index],
        .timeout   = std::numeric_limits<std::uint64_t>::max(),
    };

    auto acq_result = m_swapchain->AcquireNextImage(next_img_dets);
    if (!acq_result.has_value())
    {
        LogError("{}", acq_result.error().Message());
        return VK_NULL_HANDLE;
    }

    m_swapchain_image_index = *acq_result;

    VkCommandBuffer          cur_cmd_buf = m_frame_command_buffers[m_current_frame_index];
    VkCommandBufferBeginInfo begin_info{
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

auto RenderContext::EndFrame() -> void
{
    for (auto const& system : m_render_systems) { system->Update(); }

    m_render_graph->Compile();
    m_render_graph->Execute(m_frame_command_buffers[m_current_frame_index]);

    if (VK_SUCCESS != vkEndCommandBuffer(m_frame_command_buffers[m_current_frame_index]))
    {
        LogError("Could not record command buffer.");
        return;
    }

    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submit_info{
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = 1U,
        .pWaitSemaphores      = &m_acquire_semaphores[m_current_frame_index],
        .pWaitDstStageMask    = wait_stages,
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

    auto result = m_swapchain->QueuePresent(m_submit_semaphores[m_swapchain_image_index]);
    if (!result.has_value())
    {
        LogError("{}", result.error().Message());
        return;
    }

    m_current_frame_index = (m_current_frame_index + 1) % kMaxFramesInFlight;

    return;
}

auto RenderContext::GetRenderGraph() const -> IRenderGraph&
{
    return *m_render_graph;
}

auto RenderContext::IsInitialized() const -> bool
{
    return m_is_initialized;
}

auto RenderContext::GetSwapchain() -> VulkanSwapchain&
{
    return *m_swapchain;
}

auto RenderContext::GetCurrentFrameTexture() -> Result<std::shared_ptr<texture_2d>>
{
    if (m_swapchain_image_index > m_swapchain->GetImageCount())
    {
        return ErrorType{RenderingErrc::kFailedToCreateCurrFrameAsTexture};
    }

    return m_vrm->m_textures[static_cast<std::size_t>(m_swapchain_image_index)];
}

auto RenderContext::CreateRenderPass(render_pass_layout_description const& t_layout) -> Result<VkRenderPass>
{
    VkRenderPassCreateInfo2 pass_info{
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
        RenderingErrc::kFailedToCreateRenderPass);

    m_vk_render_passes.push_back(vk_render_pass);

    return vk_render_pass;
}

auto RenderContext::AddFramebufferToCurrentFrame(VkFramebuffer t_fbuf) -> void
{
    m_transient_fbufs[m_current_frame_index].push_back(t_fbuf);
}

}// namespace polos::rendering

polos::rendering::IRenderContext* CreateRenderContext()
{
    // Capsulated in PlatformManager
    return new polos::rendering::RenderContext{};
}
