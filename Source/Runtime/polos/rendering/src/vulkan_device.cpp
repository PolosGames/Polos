//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vulkan_device.hpp"

#include "polos/communication/error_code.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/queue_family_indices.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/vulkan_context.hpp"
#include "polos/rendering/vulkan_resource_manager.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

namespace polos::rendering
{

namespace
{

auto DecodeDriverVersion(std::uint32_t t_vendor_id, std::uint32_t t_driver_version) -> std::string
{
    if (t_vendor_id == 0x10DE)// NVIDIA
    {
        std::uint32_t driver_version = t_driver_version;

        std::uint32_t major = (driver_version >> 22) & 0x3FF;// 10 bits for major
        std::uint32_t minor = (driver_version >> 14) & 0xFF; // 6 bits for minor
        std::uint32_t patch = driver_version & 0xFFFF;       // 16 bits for patch

        return std::format("{}.{}.{}", major, minor, patch);
    }
    else if (t_vendor_id == 0x1002)// AMD
    {
        std::uint32_t driver_version = t_driver_version;

        // Extract version components for AMD
        std::uint32_t major = (driver_version >> 22) & 0x3FF;// 10 bits for major
        std::uint32_t minor = (driver_version >> 12) & 0x3FF;// 10 bits for minor
        std::uint32_t patch = driver_version & 0xFFF;        // 12 bits for patch

        return std::format("{}.{}.{}", major, minor, patch);
    }
    else
    {
        return "Not Found";
    }
}


}// namespace

VulkanDevice::VulkanDevice(GLFWwindow* t_window)
    : m_window{t_window}
{}

VulkanDevice::~VulkanDevice()
{
    LogInfo("VulkanDevice destroyed.");
}

auto VulkanDevice::Create(device_create_details const& t_info) -> Result<void>
{
    m_instance  = t_info.instance;
    m_surface   = t_info.surface;
    phys_device = t_info.phys_device;

    m_device_props = {
        .sType      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext      = nullptr,
        .properties = {},
    };

    /// ====================================================================================
    /// Print out device information
    /// ====================================================================================
    vkGetPhysicalDeviceProperties2(phys_device, &m_device_props);

    LogInfo(
        "Selected GPU:\n- Device Name: {}\n- Driver Version: {}",
        m_device_props.properties.deviceName,
        DecodeDriverVersion(m_device_props.properties.vendorID, m_device_props.properties.driverVersion));

    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(phys_device, &device_features);

    constexpr float const queue_prio{1.0f};

    std::vector<VkDeviceQueueCreateInfo> q_create_infos;

    if (t_info.q_indices.gfx_q_index != std::numeric_limits<std::uint32_t>::max())
    {
        VkDeviceQueueCreateInfo info{
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0U,
            .queueFamilyIndex = t_info.q_indices.gfx_q_index,
            .queueCount       = 1U,
            .pQueuePriorities = &queue_prio,
        };
        q_create_infos.push_back(info);
    }

    if (t_info.q_indices.transfer_q_index != std::numeric_limits<std::uint32_t>::max() &&
        // Create only if gfx queue doesn't include a VK_QUEUE_TRANSFER_BIT as well.
        t_info.q_indices.transfer_q_index != t_info.q_indices.gfx_q_index)
    {
        VkDeviceQueueCreateInfo info{
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0U,
            .queueFamilyIndex = t_info.q_indices.transfer_q_index,
            .queueCount       = 1U,
            .pQueuePriorities = &queue_prio,
        };
        q_create_infos.push_back(info);
    }

    if (t_info.q_indices.gfx_q_index != std::numeric_limits<std::uint32_t>::max() &&
        // Create only if either gfx or transfer queues doesn't include a VK_QUEUE_COMPUTE_BIT as well.
        t_info.q_indices.compute_q_index != t_info.q_indices.gfx_q_index &&
        t_info.q_indices.compute_q_index != t_info.q_indices.transfer_q_index)
    {
        VkDeviceQueueCreateInfo info{
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0U,
            .queueFamilyIndex = t_info.q_indices.gfx_q_index,
            .queueCount       = 1U,
            .pQueuePriorities = &queue_prio,
        };
        q_create_infos.push_back(info);
    }

    /// ==========================================
    ///         Logical device creation
    /// ==========================================
    VkDeviceCreateInfo device_create_info{
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .queueCreateInfoCount    = VK_SIZE_CAST(q_create_infos.size()),
        .pQueueCreateInfos       = q_create_infos.data(),
        .enabledLayerCount       = 0U,     // deprecated
        .ppEnabledLayerNames     = nullptr,// deprecated
        .enabledExtensionCount   = 1U,
        .ppEnabledExtensionNames = t_info.enabled_extensions.data(),
        .pEnabledFeatures        = &device_features,
    };

    CHECK_VK_SUCCESS_OR_ERR(
        vkCreateDevice(phys_device, &device_create_info, nullptr, &logi_device),
        RenderingErrc::kFailedDeviceCreation);

    return {};
}

auto VulkanDevice::Destroy() -> Result<void>
{
    vkDestroyDevice(logi_device, nullptr);

    return {};
}

auto VulkanDevice::CheckSurfaceFormatSupport(VkSurfaceFormatKHR const& t_required_format) const -> bool
{
    std::uint32_t format_count{0U};
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys_device, m_surface, &format_count, nullptr);

    std::vector<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys_device, m_surface, &format_count, formats.data());

    return std::ranges::any_of(formats, [&req_format = t_required_format](VkSurfaceFormatKHR const& t_format) {
        return t_format.format == req_format.format && t_format.colorSpace == req_format.colorSpace;
    });
}

auto VulkanDevice::CheckPresentModeSupport(VkPresentModeKHR const t_required_mode) const -> bool
{
    std::uint32_t mode_count{0U};
    vkGetPhysicalDeviceSurfacePresentModesKHR(phys_device, m_surface, &mode_count, nullptr);

    std::vector<VkPresentModeKHR> modes(mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(phys_device, m_surface, &mode_count, modes.data());

    return std::ranges::any_of(modes, [t_required_mode](VkPresentModeKHR const t_mode) {
        return t_mode == t_required_mode;
    });
}

}// namespace polos::rendering