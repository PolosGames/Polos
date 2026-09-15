//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//
#define VMA_IMPLEMENTATION

#include "vk/vulkan_device.hpp"

#include "polos/communication/error_code.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "vk/common.hpp"
#include "vk/queue_family_indices.hpp"
#include "vk/vulkan_resource_manager.hpp"

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

auto DecodeDriverVersion(std::uint32_t tVendorId, std::uint32_t tDriverVersion) -> std::string
{
    if (tVendorId == kVendorNvidia)
    {
        std::uint32_t const driver_version = tDriverVersion;

        std::uint32_t major = (driver_version >> 22) & 0x3FF;// NOLINT 10 bits for major
        std::uint32_t minor = (driver_version >> 14) & 0xFF; // NOLINT 6 bits for minor
        std::uint32_t patch = driver_version & 0xFFFF;       // NOLINT 16 bits for patch

        return std::format("{}.{}.{}", major, minor, patch);
    }
    if (tVendorId == kVendorAmd)// AMD
    {
        std::uint32_t const driver_version = tDriverVersion;

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

auto VulkanDevice::Create(DeviceCreateDetails const& tInfo) -> Result<void>
{
    mInstance   = tInfo.instance;
    mSurface    = tInfo.surface;
    mPhysDevice = tInfo.physDevice;

    mDeviceProps = {
        .sType      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext      = nullptr,
        .properties = {},
    };

    /// ====================================================================================
    /// Print out device information
    /// ====================================================================================
    vkGetPhysicalDeviceProperties2(mPhysDevice, &mDeviceProps);

    LogInfo("Selected GPU:\n- Device Name: {}\n- Driver Version: {}",
            mDeviceProps.properties.deviceName,
            DecodeDriverVersion(mDeviceProps.properties.vendorID, mDeviceProps.properties.driverVersion));

    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(mPhysDevice, &device_features);

    constexpr float const kQueuePrio{1.0F};

    std::vector<VkDeviceQueueCreateInfo> q_create_infos;

    if (tInfo.qIndices.gfxQIndex != std::numeric_limits<std::uint32_t>::max())
    {
        VkDeviceQueueCreateInfo const info{
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0U,
            .queueFamilyIndex = tInfo.qIndices.gfxQIndex,
            .queueCount       = 1U,
            .pQueuePriorities = &kQueuePrio,
        };
        q_create_infos.push_back(info);
    }

    if (tInfo.qIndices.transferQIndex != std::numeric_limits<std::uint32_t>::max() &&
        // Create only if gfx queue doesn't include a VK_QUEUE_TRANSFER_BIT as well.
        tInfo.qIndices.transferQIndex != tInfo.qIndices.gfxQIndex)
    {
        VkDeviceQueueCreateInfo const info{
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0U,
            .queueFamilyIndex = tInfo.qIndices.transferQIndex,
            .queueCount       = 1U,
            .pQueuePriorities = &kQueuePrio,
        };
        q_create_infos.push_back(info);
    }

    if (tInfo.qIndices.gfxQIndex != std::numeric_limits<std::uint32_t>::max() &&
        // Create only if either gfx or transfer queues doesn't include a VK_QUEUE_COMPUTE_BIT as well.
        tInfo.qIndices.computeQIndex != tInfo.qIndices.gfxQIndex &&
        tInfo.qIndices.computeQIndex != tInfo.qIndices.transferQIndex)
    {
        VkDeviceQueueCreateInfo const info{
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0U,
            .queueFamilyIndex = tInfo.qIndices.gfxQIndex,
            .queueCount       = 1U,
            .pQueuePriorities = &kQueuePrio,
        };
        q_create_infos.push_back(info);
    }

    /// ==========================================
    ///         Logical device creation
    /// ==========================================

    std::uint32_t count{0U};
    vkEnumerateDeviceExtensionProperties(mPhysDevice, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> extensions(count);
    vkEnumerateDeviceExtensionProperties(mPhysDevice, nullptr, &count, extensions.data());

    std::size_t satisifed_extensions{0U};

    for (auto const* req_ext : tInfo.enabledExtensions)
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

    assert(satisifed_extensions == tInfo.enabledExtensions.size() &&
           "Needed extensions are not supported on current device!");

    VkDeviceCreateInfo const device_create_info{
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .queueCreateInfoCount    = VK_SIZE_CAST(q_create_infos.size()),
        .pQueueCreateInfos       = q_create_infos.data(),
        .enabledLayerCount       = 0U,     // deprecated
        .ppEnabledLayerNames     = nullptr,// deprecated
        .enabledExtensionCount   = VK_SIZE_CAST(tInfo.enabledExtensions.size()),
        .ppEnabledExtensionNames = tInfo.enabledExtensions.data(),
        .pEnabledFeatures        = &device_features,
    };

    CHECK_VK_SUCCESS_OR_ERR(vkCreateDevice(mPhysDevice, &device_create_info, nullptr, &mLogiDevice),
                            RenderingErrc::kFailedCreateDevice);

    // Create VMA allocator
    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.flags            = 0U;
    allocator_info.physicalDevice   = mPhysDevice;
    allocator_info.device           = mLogiDevice;
    allocator_info.instance         = mInstance;
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_3;

    CHECK_VK_SUCCESS_OR_ERR(vmaCreateAllocator(&allocator_info, &mAllocator), RenderingErrc::kFailedCreateDevice);

    LogInfo("Created VMA allocator");

    return {};
}

auto VulkanDevice::Destroy() -> Result<void>// NOLINT
{
    LogInfo("Destroying VkDevice...");

    if (mAllocator != VK_NULL_HANDLE)
    {
        vmaDestroyAllocator(mAllocator);
        mAllocator = VK_NULL_HANDLE;
    }

    vkDestroyDevice(mLogiDevice, nullptr);

    return {};
}

auto VulkanDevice::GetLogicalDevice() const -> VkDevice
{ return mLogiDevice; }

auto VulkanDevice::GetPhysicalDevice() const -> VkPhysicalDevice
{ return mPhysDevice; }

auto VulkanDevice::GetAllocator() const -> VmaAllocator
{ return mAllocator; }

auto VulkanDevice::CheckSurfaceFormatSupport(VkSurfaceFormatKHR const& tRequiredFormat) const -> bool
{
    std::uint32_t format_count{0U};
    vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysDevice, mSurface, &format_count, nullptr);

    std::vector<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysDevice, mSurface, &format_count, formats.data());

    return std::ranges::any_of(formats, [&req_format = tRequiredFormat](VkSurfaceFormatKHR const& tFormat) {
        return tFormat.format == req_format.format && tFormat.colorSpace == req_format.colorSpace;
    });
}

auto VulkanDevice::CheckPresentModeSupport(VkPresentModeKHR tRequiredMode) const -> bool
{
    std::uint32_t mode_count{0U};
    vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysDevice, mSurface, &mode_count, nullptr);

    std::vector<VkPresentModeKHR> modes(mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysDevice, mSurface, &mode_count, modes.data());

    return std::ranges::any_of(modes, [tRequiredMode](VkPresentModeKHR const tMode) {
        return tMode == tRequiredMode;
    });
}

}// namespace polos::rendering