//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_DEVICE_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_DEVICE_HPP_

#include "polos/communication/error_code.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/module_macros.hpp"
#include "polos/rendering/queue_family_indices.hpp"

namespace polos::rendering
{

struct device_create_details
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
    explicit VulkanDevice(GLFWwindow* t_window);
    ~VulkanDevice();

    VulkanDevice(VulkanDevice&&)            = delete;
    VulkanDevice(VulkanDevice&)             = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
    VulkanDevice& operator=(VulkanDevice&)  = delete;

    auto Create(device_create_details const& t_info) -> Result<void>;
    auto Destroy() -> Result<void>;

    auto CheckSurfaceFormatSupport(VkSurfaceFormatKHR const& t_required_format) const -> bool;
    auto CheckPresentModeSupport(VkPresentModeKHR const t_mode) const -> bool;

    VkDevice         logi_device{VK_NULL_HANDLE};
    VkPhysicalDevice phys_device{VK_NULL_HANDLE};
private:
    VkInstance   m_instance{VK_NULL_HANDLE};
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};

    VkPhysicalDeviceProperties2 m_device_props;

    GLFWwindow* m_window{nullptr};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_DEVICE_HPP_
