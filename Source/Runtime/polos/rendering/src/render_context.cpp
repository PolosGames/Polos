//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/render_context.hpp"

#include "polos/rendering/common.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/vulkan_device.hpp"
#include "polos/rendering/vulkan_resource_manager.hpp"
#include "polos/rendering/vulkan_swapchain.hpp"

#include <GLFW/glfw3.h>
#include <filesystem>
#include <vulkan/vulkan_core.h>

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

RenderContext* RenderContext::s_instance       = nullptr;
bool           RenderContext::s_is_initialized = false;

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

    if (queue_index == static_cast<std::uint32_t>(queue_families.size()))
    {
        return ErrorType{RenderingErrc::kPreferableQueueFamilyNotFound};
    }

    return queue_index;
}

RenderContext::RenderContext(GLFWwindow* t_window)
    : m_window{t_window},
      m_vrm{std::make_unique<VRM>()},
      m_context{std::make_unique<VulkanContext>()},
      m_device{std::make_unique<VulkanDevice>(m_window)},
      m_swapchain{std::make_unique<VulkanSwapchain>(m_window)}
{}

auto RenderContext::Initialize() -> Result<void>
{
    // --- Initialize Vulkan context ---
    INIT_VULKAN_COMPONENT(m_context);

    // --- Surface Creation ---
    CHECK_VK_SUCCESS_OR_ERR(
        glfwCreateWindowSurface(m_context->instance, m_window, nullptr, &m_surface),
        RenderingErrc::kSurfaceNotCreated);

    // --- Physical device selection ---
    std::uint32_t device_count{0U};
    vkEnumeratePhysicalDevices(m_context->instance, &device_count, nullptr);
    if (0U != device_count)
    {
        return ErrorType{RenderingErrc::kNoPhysicalDeviceOnHost};
    }

    std::vector<VkPhysicalDevice> physical_devices(static_cast<std::size_t>(device_count));
    vkEnumeratePhysicalDevices(m_context->instance, &device_count, physical_devices.data());

    // TODO: Select the first available GPU, no need for suitability checks for now.
    VkPhysicalDevice phys_device = physical_devices[0];

    // --- Suitable queue family selection ---
    {// Graphics Family
        auto q_result = FindQueueFamily(m_device->phys_device, m_surface, VK_QUEUE_GRAPHICS_BIT);
        if (!q_result.has_value())
        {
            return ErrorType{RenderingErrc::kPreferableQueueFamilyNotFound};
        }
        m_queue_family_indices.gfx_q_index = q_result.value();
    }
    {// Transfer Family
        auto q_result = FindQueueFamily(m_device->phys_device, m_surface, VK_QUEUE_TRANSFER_BIT);
        if (!q_result.has_value())
        {
            return ErrorType{RenderingErrc::kPreferableQueueFamilyNotFound};
        }
        m_queue_family_indices.transfer_q_index = q_result.value();
    }
    {// Compute Family
        auto q_result = FindQueueFamily(m_device->phys_device, m_surface, VK_QUEUE_COMPUTE_BIT);
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
            .shader_files =
                {
                    {"Basic Color", std::filesystem::path{"Shaders/basic_color.slang.spv"}},
                },
        };

        INIT_VULKAN_COMPONENT(m_vrm, details);
    }

    return {};
}

auto RenderContext::Shutdown() -> Result<void>
{
    return {};
}

}// namespace polos::rendering
