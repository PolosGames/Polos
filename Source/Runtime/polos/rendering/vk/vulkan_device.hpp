///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_DEVICE_HPP
#define POLOS_RENDERING_VULKAN_DEVICE_HPP

#include "polos/rendering/vk/i_device.hpp"
#include "polos/rendering/vk/queue_family_indices.hpp"
#include "polos/utils/error_code.hpp"

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

#include <vector>

namespace polos::rendering
{


class VulkanDevice final : public IDevice
{
public:
    struct CreationDetails// NOLINT
    {
        std::vector<char const*> enabledExtensions;
    };

    VulkanDevice();
    ~VulkanDevice() override;

    VulkanDevice(VulkanDevice&&)            = delete;
    VulkanDevice(VulkanDevice&)             = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
    VulkanDevice& operator=(VulkanDevice&)  = delete;

    auto Create(CreationDetails const& tInfo) -> Result<void>;
    auto Destroy() -> void;

    [[nodiscard]] auto GetLogicalDevice() const -> VkDevice override;
    [[nodiscard]] auto GetPhysicalDevice() const -> VkPhysicalDevice override;
    [[nodiscard]] auto GetGraphicsQueueFamily() const -> std::uint32_t;
    [[nodiscard]] auto GetAllocator() const -> VmaAllocator override;

    [[nodiscard]] auto CheckSurfaceFormatSupport(VkSurfaceFormatKHR const& tRequiredFormat) const -> bool override;
    [[nodiscard]] auto CheckPresentModeSupport(VkPresentModeKHR tRequiredMode) const -> bool override;

    VkDevice         logiDevice{VK_NULL_HANDLE};
    VkPhysicalDevice physDevice{VK_NULL_HANDLE};
    VmaAllocator     allocator{VK_NULL_HANDLE};
    VkCommandPool    commandPool{VK_NULL_HANDLE};
    VkQueue          gfxQueue{VK_NULL_HANDLE};
private:
    VkInstance         mInstance{VK_NULL_HANDLE};
    VkSurfaceKHR       mSurface{VK_NULL_HANDLE};
    QueueFamilyIndices mQueueIndices;

    VkPhysicalDeviceProperties2 mDeviceProps{};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VULKAN_DEVICE_HPP
