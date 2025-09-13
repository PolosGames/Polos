//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/device.hpp"

#include "polos/communication/error_code.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"

#include <algorithm>
#include <cassert>
#include <set>

namespace polos::rendering
{

namespace
{

constexpr char const* kValidationLayerName = "VK_LAYER_KHRONOS_validation";
constexpr char const* kDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      t_message_severity,
    VkDebugUtilsMessageTypeFlagsEXT             t_message_type,
    VkDebugUtilsMessengerCallbackDataEXT const* t_callback_data,
    void*)
{
    std::string const message_type_str = [&]() {
        switch (t_message_type)
        {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: return "GENERAL";
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: return "VALIDATION";
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: return "PERFORMANCE";
            default: return "UNKNOWN";
        }
    }();

    switch (t_message_severity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LogWarn("Vulkan {}: {}", message_type_str, t_callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LogError("Vulkan {}: {}", message_type_str, t_callback_data->pMessage);
            break;
        default: break;
    }

    return VK_FALSE;
}

bool IsValidationLayersAvailable()
{
    std::uint32_t layer_count{0U};
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers{layer_count};
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    std::ranges::for_each(available_layers, [](VkLayerProperties const& layer) {
        LogDebug("Available Layer: {}", layer.layerName);
    });

    return std::ranges::any_of(available_layers, [](VkLayerProperties const& layer) {
        return std::strcmp(kValidationLayerName, layer.layerName) == 0;
    });
}

auto FindQueueFamily(VkPhysicalDevice t_device, VkSurfaceKHR t_surface) -> Result<std::uint32_t>
{
    std::uint32_t queue_family_count{0U};
    vkGetPhysicalDeviceQueueFamilyProperties(t_device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families{queue_family_count};
    vkGetPhysicalDeviceQueueFamilyProperties(t_device, &queue_family_count, queue_families.data());

    std::uint32_t queue_index{0U};
    for (; queue_index < queue_families.size(); ++queue_index)
    {
        VkBool32 present_support{false};
        vkGetPhysicalDeviceSurfaceSupportKHR(t_device, queue_index, t_surface, &present_support);

        VkQueueFamilyProperties const& family = queue_families[queue_index];

        // Prefer the device that supports drawing and presentation in the same queue
        if ((family.queueFlags & VK_QUEUE_GRAPHICS_BIT) && present_support)
        {
            break;
        }
    }

    if (queue_index == static_cast<std::uint32_t>(queue_families.size()))
    {
        return ErrorType{RenderingErrc::kPreferableQueueFamilyNotFound};
    }

    return queue_index;
}

// bool CheckDeviceExtensionSupport(VkPhysicalDevice t_device)
// {
//     std::uint32_t extension_count{0U};
//     vkEnumerateDeviceExtensionProperties(t_device, nullptr, &extension_count, nullptr);

//     std::vector<VkExtensionProperties> available_extensions(extension_count);
//     vkEnumerateDeviceExtensionProperties(t_device, nullptr, &extension_count, available_extensions.data());

//     std::set<std::string> required_extensions;
//     required_extensions.insert(kDeviceExtensions);

//     std::ranges::for_each(available_extensions, [&required_extensions](VkExtensionProperties const& extension) {
//         required_extensions.erase(extension.extensionName);
//     });

//     return required_extensions.empty();
// }

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
{}// namespace polos::rendering

VulkanDevice::~VulkanDevice()
{
    LogInfo("VulkanDevice destroyed.");
}

Result<void> VulkanDevice::Initialize()
{
    CHECK_VK_SUCCESS_OR_ERR(create_instance(), RenderingErrc::kInstanceNotCreated);

#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT debug_info = create_debug_messenger_create_info();
    auto                               func       = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));
    if (nullptr != func)
    {
        func(m_instance, &debug_info, nullptr, &m_dbg_messenger);
    }
    else
    {
        LogWarn("vkCreateDebugUtilsMessengerEXT function was not found!");
    }
#endif// NDEBUG

    /// ====================================================================================
    /// Select physical device
    /// ====================================================================================
    std::uint32_t device_count{0U};
    vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
    if (0U != device_count)
    {
        return ErrorType{RenderingErrc::kNoPhysicalDeviceOnHost};
    }

    std::vector<VkPhysicalDevice> physical_devices(static_cast<std::size_t>(device_count));
    vkEnumeratePhysicalDevices(m_instance, &device_count, physical_devices.data());

    // Select the first available GPU, no need for suitability checks
    m_phys_device = physical_devices[0];

    /// ====================================================================================
    /// Select Queues
    /// ====================================================================================
    auto q_result = FindQueueFamily(m_phys_device, m_surface);
    if (!q_result.has_value())
    {
        return ErrorType{RenderingErrc::kPreferableQueueFamilyNotFound};
    }

    m_queue_family_index = q_result.value();

    /// ====================================================================================
    /// Print out device information
    /// ====================================================================================
    vkGetPhysicalDeviceProperties2(m_phys_device, &m_device_props);

    LogInfo(
        "Selected GPU:\n- Device Name: {}\n- Driver Version: {}",
        m_device_props.properties.deviceName,
        DecodeDriverVersion(m_device_props.properties.vendorID, m_device_props.properties.driverVersion));

    if (!create_device())
    {
        return ErrorType{RenderingErrc::kFailedDeviceCreation};
    }

    return {};
}

auto VulkanDevice::Shutdown() -> Result<void>
{
    vkDestroyDevice(m_logi_device, nullptr);
#ifndef NDEBUG
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (func != nullptr && m_dbg_messenger != VK_NULL_HANDLE)
    {
        func(m_instance, m_dbg_messenger, nullptr);
    }
#endif// !NDEBUG
    vkDestroyInstance(m_instance, nullptr);

    return {};
}

auto VulkanDevice::create_instance() -> VkResult
{
    VkApplicationInfo app{
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext              = nullptr,
        .pApplicationName   = "Polos App",
        .applicationVersion = VK_API_VERSION_1_3,
        .pEngineName        = "Polos",
        .engineVersion      = VK_API_VERSION_1_3,
        .apiVersion         = VK_API_VERSION_1_3,
    };

    std::uint32_t glfw_extension_count{0};
    char const**  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    m_extensions.assign(glfw_extensions, std::next(glfw_extensions, static_cast<std::ptrdiff_t>(glfw_extension_count)));

#ifndef NDEBUG
    enable_validation_layers();

    VkDebugUtilsMessengerCreateInfoEXT debug_info = create_debug_messenger_create_info();
    m_instance_pnext                              = &debug_info;
#endif// NDEBUG
    VkInstanceCreateInfo instance_info{
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext                   = m_instance_pnext,
        .flags                   = 0U,
        .pApplicationInfo        = &app,
        .enabledLayerCount       = static_cast<std::uint32_t>(m_layers.size()),
        .ppEnabledLayerNames     = m_layers.data(),
        .enabledExtensionCount   = static_cast<std::uint32_t>(m_extensions.size()),
        .ppEnabledExtensionNames = m_extensions.data(),// provides VK_KHR_swapchain
    };

    return vkCreateInstance(&instance_info, nullptr, &m_instance);
}

auto VulkanDevice::create_device() -> bool
{
    constexpr float const queue_prio{1.0f};

    VkDeviceQueueCreateInfo queue_create_info{
        .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = 0U,
        .queueFamilyIndex = m_queue_family_index,
        .queueCount       = 1U,
        .pQueuePriorities = &queue_prio,
    };

    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(m_phys_device, &device_features);

    /// ==========================================
    ///         Logical device creation
    /// ==========================================
    VkDeviceCreateInfo device_create_info{
        .sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                = nullptr,
        .flags                = 0U,
        .queueCreateInfoCount = 1U,
        .pQueueCreateInfos    = &queue_create_info,
        // enabledLayerCount and ppEnabledLayerNames are deprecated and ignored by modern Vulkan implementations,
        .enabledLayerCount   = static_cast<std::uint32_t>(m_layers.size()),
        .ppEnabledLayerNames = m_layers.data(),
        // ----------------------------------------------------------------------
        .enabledExtensionCount   = 1U,
        .ppEnabledExtensionNames = &kDeviceExtensions,
        .pEnabledFeatures        = &device_features,
    };

    if (vkCreateDevice(m_phys_device, &device_create_info, nullptr, &m_logi_device) != VK_SUCCESS)
    {
        return false;
    }

    vkGetDeviceQueue(m_logi_device, m_queue_family_index, 0, &m_gfx_present_q);
    LogInfo("Graphics Queue Index: {}", m_queue_family_index);

    return true;
}

auto VulkanDevice::enable_validation_layers() -> void
{
    if (!IsValidationLayersAvailable())
    {
        LogWarn("Validation layers are not available at this time.");
        return;
    }

    // No need for enabling debug utils extension if validation layers are not available anyways.
    m_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    m_layers.push_back("VK_LAYER_KHRONOS_validation");
}

VkDebugUtilsMessengerCreateInfoEXT VulkanDevice::create_debug_messenger_create_info() const
{
    // clang-format off
    return VkDebugUtilsMessengerCreateInfoEXT{
        .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext           = nullptr,
        .flags           = 0,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = DebugCallback,
        .pUserData       = nullptr,
    };
    // clang-format on
}

}// namespace polos::rendering