//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//
#include "polos/utils/assert.hpp"
#define VMA_IMPLEMENTATION

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/resources/buffer.hpp"
#include "polos/rendering/resources/image.hpp"
#include "polos/rendering/vk/common.hpp"
#include "polos/rendering/vk/queue_family_indices.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/vulkan_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"
#include "polos/utils/error_code.hpp"

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cassert>
#include <limits>
#include <optional>
#include <span>
#include <string_view>

namespace polos::rendering
{

namespace
{

constexpr std::uint32_t const kVendorNvidia{0x10DEU};
constexpr std::uint32_t const kVendorAmd{0x1002U};

auto FindGraphicsQueueFamily(VkPhysicalDevice tDevice, VkSurfaceKHR tSurface) -> std::uint32_t
{
    constexpr VkQueueFlags required = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;

    std::uint32_t count{0U};
    vkGetPhysicalDeviceQueueFamilyProperties(tDevice, &count, nullptr);

    std::vector<VkQueueFamilyProperties> families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(tDevice, &count, families.data());

    for (std::uint32_t index = 0U; index < count; ++index)
    {
        VkBool32 canPresent{VK_FALSE};

        if ((families[index].queueFlags & required) == required &&
            vkGetPhysicalDeviceSurfaceSupportKHR(tDevice, index, tSurface, &canPresent) == VK_SUCCESS &&
            canPresent == VK_TRUE)
        {
            return index;
        }
    }
    return VK_QUEUE_FAMILY_IGNORED;
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

auto QueryDeviceFeatures(VkPhysicalDevice tDevice, VkPhysicalDeviceSynchronization2Features& tSynchronization2)
    -> VkPhysicalDeviceFeatures2
{
    VkPhysicalDeviceFeatures2 features{
        .sType    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext    = &tSynchronization2,
        .features = {},
    };
    vkGetPhysicalDeviceFeatures2(tDevice, &features);
    return features;
}

auto SupportsDeviceExtensions(VkPhysicalDevice tDevice, std::span<char const* const> tRequired) -> bool
{
    std::uint32_t count{0U};
    if (vkEnumerateDeviceExtensionProperties(tDevice, nullptr, &count, nullptr) != VK_SUCCESS)
    {
        return false;
    }
    std::vector<VkExtensionProperties> extensions(count);
    if (vkEnumerateDeviceExtensionProperties(tDevice, nullptr, &count, extensions.data()) != VK_SUCCESS)
    {
        return false;
    }

    return std::ranges::all_of(tRequired, [&extensions](std::string_view tName) {
        return std::ranges::any_of(extensions, [tName](VkExtensionProperties const& tExtension) {
            return tName == tExtension.extensionName;
        });
    });
}

struct PhysicalDeviceSelection
{
    VkPhysicalDevice           device;
    VkPhysicalDeviceProperties properties;
    std::uint32_t              graphicsQueueFamily;
};

auto SelectPhysicalDevice(VkInstance tInstance, VkSurfaceKHR tSurface, std::span<char const* const> tExtensions)
    -> std::optional<PhysicalDeviceSelection>
{
    std::uint32_t count{0U};
    if (VK_SUCCESS != vkEnumeratePhysicalDevices(tInstance, &count, nullptr) || count == 0U)
    {
        return std::nullopt;
    }

    std::vector<VkPhysicalDevice> devices(count);
    if (VK_SUCCESS != vkEnumeratePhysicalDevices(tInstance, &count, devices.data()))
    {
        return std::nullopt;
    }
    devices.resize(count);

    std::optional<PhysicalDeviceSelection> fallback;
    for (auto* const device : devices)
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(device, &properties);
        if (properties.apiVersion < VK_API_VERSION_1_3)
        {
            LogInfo(
                "Skipping GPU '{}' because it reports Vulkan {}.{}.{} and Polos requires Vulkan 1.3.",
                properties.deviceName,
                VK_API_VERSION_MAJOR(properties.apiVersion),
                VK_API_VERSION_MINOR(properties.apiVersion),
                VK_API_VERSION_PATCH(properties.apiVersion));
            continue;
        }

        VkPhysicalDeviceSynchronization2Features synchronization2{
            .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
            .pNext            = nullptr,
            .synchronization2 = VK_FALSE,
        };
        auto const features = QueryDeviceFeatures(device, synchronization2).features;
        if (!synchronization2.synchronization2 || !features.multiDrawIndirect || !features.drawIndirectFirstInstance)
        {
            LogInfo(
                "Skipping GPU '{}' because it lacks required synchronization or indirect draw features.",
                properties.deviceName);
            continue;
        }
        if (!SupportsDeviceExtensions(device, tExtensions))
        {
            LogInfo("Skipping GPU '{}' because it lacks a required device extension.", properties.deviceName);
            continue;
        }

        // Compute dispatch, rendering and presentation all use the same graphics queue.
        auto const graphics = FindGraphicsQueueFamily(device, tSurface);
        if (graphics == VK_QUEUE_FAMILY_IGNORED)
        {
            LogInfo(
                "Skipping GPU '{}' because it has no graphics/compute queue with presentation support.",
                properties.deviceName);
            continue;
        }

        PhysicalDeviceSelection const selected{
            .device              = device,
            .properties          = properties,
            .graphicsQueueFamily = graphics,
        };
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            return selected;
        }
        if (!fallback)
        {
            fallback = selected;
        }
    }

    return fallback;
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

    auto const selected = SelectPhysicalDevice(mInstance, mSurface, tInfo.enabledExtensions);
    if (!selected)
    {
        return ErrorType{RenderingErrc::kFailedFindPhysDevice};
    }

    physDevice              = selected->device;
    mQueueIndices.gfxQIndex = selected->graphicsQueueFamily;
    auto const& properties  = selected->properties;
    LogInfo(
        "Selected GPU:\n- Device Name: {}\n- Driver Version: {}\n- Vulkan API Version: {}.{}.{}",
        properties.deviceName,
        DecodeDriverVersion(properties.vendorID, properties.driverVersion),
        VK_API_VERSION_MAJOR(properties.apiVersion),
        VK_API_VERSION_MINOR(properties.apiVersion),
        VK_API_VERSION_PATCH(properties.apiVersion));

    VkPhysicalDeviceSynchronization2Features synchronization2_features{
        .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
        .pNext            = nullptr,
        .synchronization2 = VK_FALSE,
    };
    auto const device_features = QueryDeviceFeatures(physDevice, synchronization2_features);

    constexpr float const         kQueuePrio{1.0F};
    VkDeviceQueueCreateInfo const queue_info{
        .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = 0U,
        .queueFamilyIndex = mQueueIndices.gfxQIndex,
        .queueCount       = 1U,
        .pQueuePriorities = &kQueuePrio,
    };

    VkDeviceCreateInfo const device_create_info{
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                   = &device_features,
        .flags                   = 0U,
        .queueCreateInfoCount    = 1U,
        .pQueueCreateInfos       = &queue_info,
        .enabledLayerCount       = 0U,     // deprecated
        .ppEnabledLayerNames     = nullptr,// deprecated
        .enabledExtensionCount   = ToVkSize(tInfo.enabledExtensions.size()),
        .ppEnabledExtensionNames = tInfo.enabledExtensions.data(),
        .pEnabledFeatures        = nullptr,
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