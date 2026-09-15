///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_VK_VULKAN_CONTEXT_HPP
#define POLOS_RENDERING_SRC_VK_VULKAN_CONTEXT_HPP

#include "polos/communication/error_code.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace polos::rendering
{

struct alignas(32) ContextCreateDetails// NOLINT
{
    std::vector<char const*> requiredExtensions;
};

class VulkanContext
{
public:
    VulkanContext();
    ~VulkanContext();

    VulkanContext(VulkanContext const&)            = delete;
    VulkanContext(VulkanContext&&)                 = delete;
    VulkanContext& operator=(VulkanContext const&) = delete;
    VulkanContext& operator=(VulkanContext&&)      = delete;

    auto Create(ContextCreateDetails const& tDetails) -> Result<void>;
    auto Destroy() -> Result<void>;

    VkInstance mInstance{VK_NULL_HANDLE};
private:
    auto create_instance() -> VkResult;
    auto enableValidationLayers() -> void;

    VkDebugUtilsMessengerEXT mDbgMessenger{VK_NULL_HANDLE};

    std::vector<char const*> mLayers;
    std::vector<char const*> mExtensions;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_VK_VULKAN_CONTEXT_HPP
