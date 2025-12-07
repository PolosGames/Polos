//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//
#define VMA_IMPLEMENTATION

#include "polos/rendering/vulkan_device.hpp"

#include "polos/communication/error_code.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/queue_family_indices.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/vulkan_resource_manager.hpp"

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cassert>
#include <limits>

namespace polos::rendering
{

namespace
{

constexpr std::uint32_t const kVendorNvidia{0x10DEU};
constexpr std::uint32_t const kVendorAmd{0x1002U};

auto DecodeDriverVersion(std::uint32_t t_vendor_id, std::uint32_t t_driver_version) -> std::string
{
    if (t_vendor_id == kVendorNvidia)
    {
        std::uint32_t const driver_version = t_driver_version;

        std::uint32_t major = (driver_version >> 22) & 0x3FF;// NOLINT 10 bits for major
        std::uint32_t minor = (driver_version >> 14) & 0xFF; // NOLINT 6 bits for minor
        std::uint32_t patch = driver_version & 0xFFFF;       // NOLINT 16 bits for patch

        return std::format("{}.{}.{}", major, minor, patch);
    }
    if (t_vendor_id == kVendorAmd)// AMD
    {
        std::uint32_t const driver_version = t_driver_version;

        // Extract version components for AMD
        std::uint32_t major = (driver_version >> 22U) & 0x3FF;// NOLINT 10 bits for major
        std::uint32_t minor = (driver_version >> 12U) & 0x3FF;// NOLINT 10 bits for minor
        std::uint32_t patch = driver_version & 0xFFF;         // NOLINT 12 bits for patch

        return std::format("{}.{}.{}", major, minor, patch);
    }
    return "Not Found";
}


}// namespace

VulkanDevice::VulkanDevice()  = default;
VulkanDevice::~VulkanDevice() = default;

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

    constexpr float const queue_prio{1.0F};

    std::vector<VkDeviceQueueCreateInfo> q_create_infos;

    if (t_info.q_indices.gfx_q_index != std::numeric_limits<std::uint32_t>::max())
    {
        VkDeviceQueueCreateInfo const info{
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
        VkDeviceQueueCreateInfo const info{
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
        VkDeviceQueueCreateInfo const info{
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

    std::uint32_t count{0U};
    vkEnumerateDeviceExtensionProperties(phys_device, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> extensions(count);
    vkEnumerateDeviceExtensionProperties(phys_device, nullptr, &count, extensions.data());

    std::size_t satisifed_extensions{0U};

    for (auto const* req_ext : t_info.enabled_extensions)
    {
        for (auto const& device_ext : extensions)
        {
            if (std::string_view{&device_ext.extensionName[0]} == std::string_view{req_ext})
            {
                ++satisifed_extensions;
                break;
            }
        }
    }

    assert(
        satisifed_extensions == t_info.enabled_extensions.size() &&
        "Needed extensions are not supported on current device!");

    VkDeviceCreateInfo const device_create_info{
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .queueCreateInfoCount    = VK_SIZE_CAST(q_create_infos.size()),
        .pQueueCreateInfos       = q_create_infos.data(),
        .enabledLayerCount       = 0U,     // deprecated
        .ppEnabledLayerNames     = nullptr,// deprecated
        .enabledExtensionCount   = VK_SIZE_CAST(t_info.enabled_extensions.size()),
        .ppEnabledExtensionNames = t_info.enabled_extensions.data(),
        .pEnabledFeatures        = &device_features,
    };

    CHECK_VK_SUCCESS_OR_ERR(
        vkCreateDevice(phys_device, &device_create_info, nullptr, &logi_device),
        RenderingErrc::kFailedCreateDevice);

    // Create VMA allocator
    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.flags            = 0U;
    allocator_info.physicalDevice   = phys_device;
    allocator_info.device           = logi_device;
    allocator_info.instance         = m_instance;
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_3;

    CHECK_VK_SUCCESS_OR_ERR(vmaCreateAllocator(&allocator_info, &allocator), RenderingErrc::kFailedCreateDevice);

    LogInfo("Created VMA allocator");

    return {};
}

auto VulkanDevice::Destroy() -> Result<void>// NOLINT
{
    LogInfo("Destroying VkDevice...");

    if (allocator != VK_NULL_HANDLE)
    {
        vmaDestroyAllocator(allocator);
        allocator = VK_NULL_HANDLE;
    }

    vkDestroyDevice(logi_device, nullptr);

    return {};
}

auto VulkanDevice::GetLogicalDevice() const -> VkDevice
{
    return logi_device;
}

auto VulkanDevice::GetPhysicalDevice() const -> VkPhysicalDevice
{
    return phys_device;
}

auto VulkanDevice::GetAllocator() const -> VmaAllocator
{
    return allocator;
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

auto VulkanDevice::CheckPresentModeSupport(VkPresentModeKHR t_required_mode) const -> bool
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