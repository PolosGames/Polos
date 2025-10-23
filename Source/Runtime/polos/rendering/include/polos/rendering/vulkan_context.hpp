//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VULKAN_CONTEXT_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VULKAN_CONTEXT_HPP_

#include "polos/communication/error_code.hpp"
#include "polos/rendering/common.hpp"

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

    VkInstance instance;
private:
    auto create_instance() -> VkResult;
    auto enable_validation_layers() -> void;
    auto create_debug_messenger_create_info() const -> VkDebugUtilsMessengerCreateInfoEXT;

    void const* m_instance_pnext{nullptr};

    VkDebugUtilsMessengerEXT m_dbg_messenger{VK_NULL_HANDLE};

    std::vector<char const*> m_layers;
    std::vector<char const*> m_extensions;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VULKAN_CONTEXT_HPP_
