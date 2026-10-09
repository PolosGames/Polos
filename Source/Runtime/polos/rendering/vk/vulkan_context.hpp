///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_CONTEXT_HPP
#define POLOS_RENDERING_VULKAN_CONTEXT_HPP

#include "polos/utils/error_code.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace polos::platform
{
class IWindowSurface;
}// namespace polos::platform

namespace polos::rendering
{

class VulkanContext
{
public:
    struct CreationDetails
    { platform::IWindowSurface* windowSurface; };

    VulkanContext();
    ~VulkanContext();

    VulkanContext(VulkanContext const&)            = delete;
    VulkanContext(VulkanContext&&)                 = delete;
    VulkanContext& operator=(VulkanContext const&) = delete;
    VulkanContext& operator=(VulkanContext&&)      = delete;

    [[nodiscard]] auto Create(CreationDetails const& tDetails) -> Result<void>;
    auto               Destroy() -> void;

    VkInstance   mInstance{VK_NULL_HANDLE};
    VkSurfaceKHR surface{VK_NULL_HANDLE};
private:
    auto enableValidationLayers() -> void;

    VkDebugUtilsMessengerEXT mDbgMessenger{VK_NULL_HANDLE};

    std::vector<char const*> mLayers;
    std::vector<char const*> mExtensions;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VULKAN_CONTEXT_HPP
