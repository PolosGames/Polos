///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_DEVICE_HPP
#define POLOS_RENDERING_VULKAN_DEVICE_HPP

#include "polos/communication/error_code.hpp"
#include "polos/rendering/i_device.hpp"
#include "polos/rendering/queue_family_indices.hpp"

#include <vk_mem_alloc.h>

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

class VulkanDevice final : public IDevice
{
public:
    VulkanDevice();
    ~VulkanDevice() override;

    VulkanDevice(VulkanDevice&&)            = delete;
    VulkanDevice(VulkanDevice&)             = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
    VulkanDevice& operator=(VulkanDevice&)  = delete;

    auto Create(device_create_details const& t_info) -> Result<void>;
    auto Destroy() -> Result<void>;

    [[nodiscard]] auto GetLogicalDevice() const -> VkDevice override;
    [[nodiscard]] auto GetPhysicalDevice() const -> VkPhysicalDevice override;
    [[nodiscard]] auto GetAllocator() const -> VmaAllocator override;

    [[nodiscard]] auto CheckSurfaceFormatSupport(VkSurfaceFormatKHR const& t_required_format) const -> bool override;
    [[nodiscard]] auto CheckPresentModeSupport(VkPresentModeKHR t_required_mode) const -> bool override;

    VkDevice         logi_device{VK_NULL_HANDLE};
    VkPhysicalDevice phys_device{VK_NULL_HANDLE};
    VmaAllocator     allocator{VK_NULL_HANDLE};
private:
    VkInstance   m_instance{VK_NULL_HANDLE};
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};

    VkPhysicalDeviceProperties2 m_device_props{};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VULKAN_DEVICE_HPP
