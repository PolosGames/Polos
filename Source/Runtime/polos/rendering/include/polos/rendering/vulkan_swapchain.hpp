//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_VULKAN_SWAPCHAIN_HPP_
#define POLOS_RENDERING_INCLUDE_VULKAN_SWAPCHAIN_HPP_

#include "polos/rendering/common.hpp"
#include "polos/rendering/module_macros.hpp"

#include <vector>

namespace polos::rendering
{

class VulkanSwapchain
{
public:
    VulkanSwapchain(GLFWwindow* t_window);
    ~VulkanSwapchain();

    VulkanSwapchain(VulkanSwapchain const&)            = delete;
    VulkanSwapchain(VulkanSwapchain&&)                 = delete;
    VulkanSwapchain& operator=(VulkanSwapchain const&) = delete;
    VulkanSwapchain& operator=(VulkanSwapchain&&)      = delete;
private:
    struct surface_details
    {
        VkSurfaceCapabilitiesKHR        surface_capabilities;
        std::vector<VkSurfaceFormatKHR> surface_formats;
        std::vector<VkPresentModeKHR>   present_modes;
    };

    auto query_device_surface_details(VkPhysicalDevice t_device) -> bool;

    VkInstance      m_instance{VK_NULL_HANDLE};
    VkSurfaceKHR    m_surface{VK_NULL_HANDLE};
    VkDevice        m_device{VK_NULL_HANDLE};
    surface_details m_surface_details;

    GLFWwindow* m_window{nullptr};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_VULKAN_SWAPCHAIN_HPP_
