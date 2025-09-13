//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_DEVICE_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_DEVICE_HPP_

#include "polos/communication/error_code.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/module_macros.hpp"

#include <span>
#include <vector>

namespace polos::rendering
{

class RENDERING_EXPORT VulkanDevice
{
public:
    explicit VulkanDevice(GLFWwindow* t_window);
    ~VulkanDevice();

    VulkanDevice(VulkanDevice&&)            = delete;
    VulkanDevice(VulkanDevice&)             = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
    VulkanDevice& operator=(VulkanDevice&)  = delete;

    auto Initialize() -> Result<void>;
    auto Shutdown() -> Result<void>;

    VkInstance       GetInstance() const;
    VkDevice         GetDevice() const;
    VkPhysicalDevice GetPhysicalDevice() const;
private:
    auto create_instance() -> VkResult;
    auto create_device() -> bool;
    auto enable_validation_layers() -> void;
    auto select_physical_device(std::span<VkPhysicalDevice> t_devices) -> Result<void>;
    auto create_debug_messenger_create_info() const -> VkDebugUtilsMessengerCreateInfoEXT;

    VkInstance  m_instance{VK_NULL_HANDLE};
    void const* m_instance_pnext{nullptr};

    VkDevice                    m_logi_device{VK_NULL_HANDLE};
    VkPhysicalDevice            m_phys_device{VK_NULL_HANDLE};
    VkPhysicalDeviceProperties2 m_device_props;
    VkSurfaceKHR                m_surface{VK_NULL_HANDLE};

    VkQueue       m_gfx_present_q{VK_NULL_HANDLE};
    std::uint32_t m_queue_family_index;

    GLFWwindow* m_window{nullptr};

    VkDebugUtilsMessengerEXT m_dbg_messenger{VK_NULL_HANDLE};

    std::vector<char const*> m_layers;
    std::vector<char const*> m_extensions;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_DEVICE_HPP_
