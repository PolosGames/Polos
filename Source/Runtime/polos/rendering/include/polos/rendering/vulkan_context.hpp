///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_CONTEXT_HPP
#define POLOS_RENDERING_VULKAN_CONTEXT_HPP

#include "polos/communication/error_code.hpp"

#include <vulkan/vulkan.h>

namespace polos::rendering
{

class VulkanContext
{
public:
    VulkanContext();
    ~VulkanContext();

    VulkanContext(VulkanContext const&)            = delete;
    VulkanContext(VulkanContext&&)                 = delete;
    VulkanContext& operator=(VulkanContext const&) = delete;
    VulkanContext& operator=(VulkanContext&&)      = delete;

    auto Create() -> Result<void>;
    auto Destroy() -> Result<void>;

    VkInstance instance{VK_NULL_HANDLE};
private:
    auto create_instance() -> VkResult;
    auto enable_validation_layers() -> void;

    void const* m_instance_pnext{nullptr};

    VkDebugUtilsMessengerEXT m_dbg_messenger{VK_NULL_HANDLE};

    std::vector<char const*> m_layers;
    std::vector<char const*> m_extensions;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VULKAN_CONTEXT_HPP
