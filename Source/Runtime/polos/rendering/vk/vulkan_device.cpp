//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//
#define VMA_IMPLEMENTATION

#include "polos/rendering/vk/vulkan_device.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/resources/buffer.hpp"
#include "polos/rendering/resources/image.hpp"
#include "polos/rendering/vk/common.hpp"
#include "polos/rendering/vk/queue_family_indices.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/vulkan_context.hpp"
#include "polos/utils/error_code.hpp"

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

// VK_QUEUE_FAMILY_IGNORED is not a valid queue-family index.
auto FindQueueFamily(VkPhysicalDevice tDevice, VkSurfaceKHR tSurface, VkQueueFlags tFlags) -> std::uint32_t
{
    std::uint32_t queue_family_count{0U};
    vkGetPhysicalDeviceQueueFamilyProperties(tDevice, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families{queue_family_count};
    vkGetPhysicalDeviceQueueFamilyProperties(tDevice, &queue_family_count, queue_families.data());

    std::uint32_t queue_index{0U};
    auto const    queue_families_size = static_cast<std::uint32_t>(queue_families.size());
    for (; queue_index < queue_families_size; ++queue_index)
    {
        VkQueueFamilyProperties const& family = queue_families[queue_index];

        if (0U != (family.queueFlags & tFlags))
        {
            if (0U != (tFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                // If gfx family is requested, also check if it has present capabilities
                VkBool32 present_support{0U};
                vkGetPhysicalDeviceSurfaceSupportKHR(tDevice, queue_index, tSurface, &present_support);

                if (0U != present_support)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    if (queue_index == VK_SIZE_CAST(queue_families.size()))
    {
        LogError("No adequate queue family found for queue flags {}.", tFlags);
        return VK_QUEUE_FAMILY_IGNORED;
    }

    return queue_index;
}

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

auto CreatePhysDevice(VkInstance tInstance) -> VkPhysicalDevice
{
    // --- Physical device selection ---
    std::uint32_t device_count{0U};
    if (VK_SUCCESS != vkEnumeratePhysicalDevices(tInstance, &device_count, nullptr))
    {
        return nullptr;
    }
    if (device_count == 0U)
    {
        return nullptr;
    }

    std::vector<VkPhysicalDevice> physical_devices(static_cast<std::size_t>(device_count));
    if (VK_SUCCESS != vkEnumeratePhysicalDevices(tInstance, &device_count, physical_devices.data()))
    {
        return nullptr;
    }

    // TODO(sorbatdev): Select the first available GPU, no need for suitability checks for now.

    return physical_devices[0];
}

auto CreateQueueIndices(VkPhysicalDevice tPhysDevice, VkSurfaceKHR tSurface) -> Result<QueueFamilyIndices>
{
    QueueFamilyIndices indices;
    // --- Suitable queue family selection ---
    {// Graphics Family
        auto const q_result = FindQueueFamily(tPhysDevice, tSurface, VK_QUEUE_GRAPHICS_BIT);
        if (q_result == VK_QUEUE_FAMILY_IGNORED)
        {
            return ErrorType{RenderingErrc::kNoAdequateQueueFamily};
        }
        indices.gfxQIndex = q_result;
    }
    {// Transfer Family
        auto const q_result = FindQueueFamily(tPhysDevice, tSurface, VK_QUEUE_TRANSFER_BIT);
        if (q_result == VK_QUEUE_FAMILY_IGNORED)
        {
            return ErrorType{RenderingErrc::kNoAdequateQueueFamily};
        }
        indices.transferQIndex = q_result;
    }
    {// Compute Family
        auto const q_result = FindQueueFamily(tPhysDevice, tSurface, VK_QUEUE_COMPUTE_BIT);
        if (q_result == VK_QUEUE_FAMILY_IGNORED)
        {
            return ErrorType{RenderingErrc::kNoAdequateQueueFamily};
        }

        indices.computeQIndex = q_result;
    }

    return indices;
}

auto CreateCommandPool(VmaAllocator tAllocator, std::uint32_t const tGfxQ, VkDevice tDevice) -> VkCommandPool
{
    Image::SetAllocator(tAllocator);
    Buffer::SetAllocator(tAllocator);

    // --- Command Pool creation ---
    VkCommandPoolCreateInfo const pool_info{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = tGfxQ,
    };

    VkCommandPool pool{VK_NULL_HANDLE};
    if (VK_SUCCESS != vkCreateCommandPool(tDevice, &pool_info, nullptr, &pool))
    {
        return nullptr;
    }

    return pool;
}

}// namespace

VulkanDevice::VulkanDevice()  = default;
VulkanDevice::~VulkanDevice() = default;

auto VulkanDevice::Create(CreationDetails const& tInfo) -> Result<void>
{
    mInstance = RenderContext::GetVulkanContext().mInstance;
    mSurface  = RenderContext::GetVulkanContext().surface;

    if (physDevice = CreatePhysDevice(mInstance); VK_NULL_HANDLE == physDevice)
    {
        return ErrorType{RenderingErrc::kFailedFindPhysDevice};
    }

    auto res = CreateQueueIndices(physDevice, mSurface);
    if (!res.has_value())
    {
        return ErrorType{RenderingErrc::kNoAdequateQueueFamily};
    }
    mQueueIndices = *res;

    mDeviceProps = {
        .sType      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext      = nullptr,
        .properties = {},
    };

    /// ====================================================================================
    /// Print out device information
    /// ====================================================================================
    vkGetPhysicalDeviceProperties2(physDevice, &mDeviceProps);

    LogInfo(
        "Selected GPU:\n- Device Name: {}\n- Driver Version: {}",
        mDeviceProps.properties.deviceName,
        DecodeDriverVersion(mDeviceProps.properties.vendorID, mDeviceProps.properties.driverVersion));

    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(physDevice, &device_features);

    constexpr float const kQueuePrio{1.0F};

    std::vector<VkDeviceQueueCreateInfo> q_create_infos;

    if (mQueueIndices.gfxQIndex != std::numeric_limits<std::uint32_t>::max())
    {
        VkDeviceQueueCreateInfo const info{
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0U,
            .queueFamilyIndex = mQueueIndices.gfxQIndex,
            .queueCount       = 1U,
            .pQueuePriorities = &kQueuePrio,
        };
        q_create_infos.push_back(info);
    }

    if (mQueueIndices.transferQIndex != std::numeric_limits<std::uint32_t>::max() &&
        // Create only if gfx queue doesn't include a VK_QUEUE_TRANSFER_BIT as well.
        mQueueIndices.transferQIndex != mQueueIndices.gfxQIndex)
    {
        VkDeviceQueueCreateInfo const info{
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0U,
            .queueFamilyIndex = mQueueIndices.transferQIndex,
            .queueCount       = 1U,
            .pQueuePriorities = &kQueuePrio,
        };
        q_create_infos.push_back(info);
    }

    if (mQueueIndices.gfxQIndex != std::numeric_limits<std::uint32_t>::max() &&
        // Create only if either gfx or transfer queues doesn't include a VK_QUEUE_COMPUTE_BIT as well.
        mQueueIndices.computeQIndex != mQueueIndices.gfxQIndex &&
        mQueueIndices.computeQIndex != mQueueIndices.transferQIndex)
    {
        VkDeviceQueueCreateInfo const info{
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0U,
            .queueFamilyIndex = mQueueIndices.gfxQIndex,
            .queueCount       = 1U,
            .pQueuePriorities = &kQueuePrio,
        };
        q_create_infos.push_back(info);
    }

    /// ==========================================
    ///         Logical device creation
    /// ==========================================

    std::uint32_t count{0U};
    vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> extensions(count);
    vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &count, extensions.data());

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

    if (satisifed_extensions != tInfo.enabledExtensions.size())
    {
        return ErrorType{RenderingErrc::kNoAdequateExtension};
    }

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

    CHECK_VK_SUCCESS_OR_ERR(
        vkCreateDevice(physDevice, &device_create_info, nullptr, &logiDevice),
        RenderingErrc::kFailedCreateDevice);

    // Create VMA allocator
    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.flags            = 0U;
    allocator_info.physicalDevice   = physDevice;
    allocator_info.device           = logiDevice;
    allocator_info.instance         = mInstance;
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_3;

    if (VK_SUCCESS != vmaCreateAllocator(&allocator_info, &allocator))
    {
        Destroy();
        return ErrorType{RenderingErrc::kFailedCreateDevice};
    }

    LogDebug("Created VMA allocator");

    vkGetDeviceQueue(logiDevice, mQueueIndices.gfxQIndex, 0U, &gfxQueue);

    if (commandPool = CreateCommandPool(allocator, mQueueIndices.gfxQIndex, logiDevice); VK_NULL_HANDLE == commandPool)
    {
        Destroy();
        return ErrorType{RenderingErrc::kFailedCreateCmdPool};
    }

    return {};
}

auto VulkanDevice::Destroy() -> void
{
    LogInfo("Destroying VkDevice...");

    if (mSurface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
        mSurface = VK_NULL_HANDLE;
    }
    if (allocator != VK_NULL_HANDLE)
    {
        vmaDestroyAllocator(allocator);
        allocator = VK_NULL_HANDLE;
    }
    if (commandPool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(logiDevice, commandPool, nullptr);
        commandPool = VK_NULL_HANDLE;
    }
    if (logiDevice != VK_NULL_HANDLE)
    {
        vkDestroyDevice(logiDevice, nullptr);
        logiDevice = VK_NULL_HANDLE;
    }
    Image::SetAllocator(VK_NULL_HANDLE);
    Buffer::SetAllocator(VK_NULL_HANDLE);
    physDevice    = VK_NULL_HANDLE;
    mInstance     = VK_NULL_HANDLE;
    mQueueIndices = {};
}

auto VulkanDevice::GetLogicalDevice() const -> VkDevice
{
    return logiDevice;
}

auto VulkanDevice::GetPhysicalDevice() const -> VkPhysicalDevice
{
    return physDevice;
}

auto VulkanDevice::GetGraphicsQueueFamily() const -> std::uint32_t
{
    return mQueueIndices.gfxQIndex;
}

auto VulkanDevice::GetAllocator() const -> VmaAllocator
{
    return allocator;
}

auto VulkanDevice::CheckSurfaceFormatSupport(VkSurfaceFormatKHR const& tRequiredFormat) const -> bool
{
    std::uint32_t format_count{0U};
    vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, mSurface, &format_count, nullptr);

    std::vector<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, mSurface, &format_count, formats.data());

    return std::ranges::any_of(formats, [&req_format = tRequiredFormat](VkSurfaceFormatKHR const& tFormat) {
        return tFormat.format == req_format.format && tFormat.colorSpace == req_format.colorSpace;
    });
}

auto VulkanDevice::CheckPresentModeSupport(VkPresentModeKHR tRequiredMode) const -> bool
{
    std::uint32_t mode_count{0U};
    vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, mSurface, &mode_count, nullptr);

    std::vector<VkPresentModeKHR> modes(mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, mSurface, &mode_count, modes.data());

    return std::ranges::any_of(modes, [tRequiredMode](VkPresentModeKHR const tMode) {
        return tMode == tRequiredMode;
    });
}

}// namespace polos::rendering