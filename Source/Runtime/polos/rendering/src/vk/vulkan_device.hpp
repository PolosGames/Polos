///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_VK_VULKAN_DEVICE_HPP
#define POLOS_RENDERING_SRC_VK_VULKAN_DEVICE_HPP

#include "polos/communication/error_code.hpp"
#include "vk/i_device.hpp"
#include "vk/queue_family_indices.hpp"

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

namespace polos::rendering
{

struct alignas(64) DeviceCreateDetails// NOLINT
{
    VkInstance       instance;
    VkSurfaceKHR     surface;
    VkPhysicalDevice physDevice;

    QueueFamilyIndices       qIndices;
    std::vector<char const*> enabledExtensions;
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

    auto Create(DeviceCreateDetails const& tInfo) -> Result<void>;
    auto Destroy() -> Result<void>;

    [[nodiscard]] auto GetLogicalDevice() const -> VkDevice override;
    [[nodiscard]] auto GetPhysicalDevice() const -> VkPhysicalDevice override;
    [[nodiscard]] auto GetAllocator() const -> VmaAllocator override;

    [[nodiscard]] auto CheckSurfaceFormatSupport(VkSurfaceFormatKHR const& tRequiredFormat) const -> bool override;
    [[nodiscard]] auto CheckPresentModeSupport(VkPresentModeKHR tRequiredMode) const -> bool override;

    VkDevice         mLogiDevice{VK_NULL_HANDLE};
    VkPhysicalDevice mPhysDevice{VK_NULL_HANDLE};
    VmaAllocator     mAllocator{VK_NULL_HANDLE};
private:
    VkInstance   mInstance{VK_NULL_HANDLE};
    VkSurfaceKHR mSurface{VK_NULL_HANDLE};

    VkPhysicalDeviceProperties2 mDeviceProps{};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_VK_VULKAN_DEVICE_HPP
