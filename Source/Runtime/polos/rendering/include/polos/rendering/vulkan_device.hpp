///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_DEVICE_HPP
#define POLOS_RENDERING_VULKAN_DEVICE_HPP

#include "polos/communication/error_code.hpp"
#include "polos/rendering/module_macros.hpp"
#include "polos/rendering/queue_family_indices.hpp"

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace polos::rendering
{

struct alignas(64) device_create_details// NOLINT
{
    VkInstance       instance;
    VkSurfaceKHR     surface;
    VkPhysicalDevice phys_device;

    queue_family_indices     q_indices;
    std::vector<char const*> enabled_extensions;
};

class RENDERING_EXPORT VulkanDevice
{
public:
    VulkanDevice();
    ~VulkanDevice();

    VulkanDevice(VulkanDevice&&)            = delete;
    VulkanDevice(VulkanDevice&)             = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
    VulkanDevice& operator=(VulkanDevice&)  = delete;

    auto Create(device_create_details const& t_info) -> Result<void>;
    auto Destroy() -> Result<void>;

    [[nodiscard]] auto CheckSurfaceFormatSupport(VkSurfaceFormatKHR const& t_required_format) const -> bool;
    [[nodiscard]] auto CheckPresentModeSupport(VkPresentModeKHR t_required_mode) const -> bool;

    VkDevice         logi_device{VK_NULL_HANDLE};
    VkPhysicalDevice phys_device{VK_NULL_HANDLE};
private:
    VkInstance   m_instance{VK_NULL_HANDLE};
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};

    VkPhysicalDeviceProperties2 m_device_props{};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VULKAN_DEVICE_HPP
