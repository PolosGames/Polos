///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_VK_I_DEVICE_HPP
#define POLOS_RENDERING_SRC_VK_I_DEVICE_HPP

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

namespace polos::rendering
{

class IDevice
{
public:
    virtual ~IDevice() = default;

    [[nodiscard]] virtual auto GetLogicalDevice() const -> VkDevice          = 0;
    [[nodiscard]] virtual auto GetPhysicalDevice() const -> VkPhysicalDevice = 0;
    [[nodiscard]] virtual auto GetAllocator() const -> VmaAllocator          = 0;

    [[nodiscard]] virtual auto CheckSurfaceFormatSupport(VkSurfaceFormatKHR const& tFormat) const -> bool = 0;
    [[nodiscard]] virtual auto CheckPresentModeSupport(VkPresentModeKHR tMode) const -> bool              = 0;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_VK_I_DEVICE_HPP
