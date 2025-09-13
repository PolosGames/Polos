//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vulkan_swapchain.hpp"

#include <cstdint>

namespace polos::rendering
{

VulkanSwapchain::VulkanSwapchain(GLFWwindow* t_window)
    : m_window(t_window)
{}

VulkanSwapchain::~VulkanSwapchain()
{
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
}

auto VulkanSwapchain::query_device_surface_details(VkPhysicalDevice t_device) -> bool
{
    surface_details details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(t_device, m_surface, &details.surface_capabilities);

    std::uint32_t surface_format_count{0U};
    vkGetPhysicalDeviceSurfaceFormatsKHR(t_device, m_surface, &surface_format_count, nullptr);

    std::uint32_t present_mode_count{0U};
    vkGetPhysicalDeviceSurfacePresentModesKHR(t_device, m_surface, &present_mode_count, nullptr);

    if (0U == surface_format_count || 0U == present_mode_count)
    {
        return false;
    }

    details.surface_formats.resize(surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(t_device, m_surface, &surface_format_count, details.surface_formats.data());

    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(t_device, m_surface, &present_mode_count, details.present_modes.data());

    m_surface_details = std::move(details);

    return true;
}


}// namespace polos::rendering
