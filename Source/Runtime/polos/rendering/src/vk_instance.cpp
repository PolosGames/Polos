///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk_instance.hpp"

#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <set>
#include <span>
#include <vector>

namespace polos::rendering
{

namespace
{

constexpr std::float_t const kPolosRed{0.50980395F};
constexpr std::float_t const kPolosGreen{0.59607846F};
constexpr std::float_t const kPolosBlue{0.6431373F};

VkExtent2D g_extent{};
VkViewport g_viewport{};
VkRect2D   g_scissor{};

struct swap_chain_support_details
{
    VkSurfaceCapabilitiesKHR        surface_capabilities;
    std::vector<VkSurfaceFormatKHR> surface_formats;
    std::vector<VkPresentModeKHR>   present_modes;
};

VkInstance                  g_instance{VK_NULL_HANDLE};
VkDevice                    g_logical_device{VK_NULL_HANDLE};
VkQueue                     g_gfx_present_queue{VK_NULL_HANDLE};
VkSurfaceKHR                g_surface{VK_NULL_HANDLE};
VkPhysicalDevice            g_phys_device{VK_NULL_HANDLE};
VkPhysicalDeviceProperties2 g_device_properties{
    .sType      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
    .pNext      = nullptr,
    .properties = {},
};

GLFWwindow* g_window{nullptr};

VkSwapchainKHR             g_swap_chain{VK_NULL_HANDLE};
std::vector<VkImageView>   g_swap_chain_image_views;
std::vector<VkFramebuffer> g_swap_chain_framebuffers;
VkShaderModule             g_vertex_shader_module{VK_NULL_HANDLE};
VkShaderModule             g_fragment_shader_module{VK_NULL_HANDLE};
VkPipelineLayout           g_pipeline_layout{VK_NULL_HANDLE};
VkRenderPass               g_render_pass{VK_NULL_HANDLE};
VkPipeline                 g_graphics_pipeline{VK_NULL_HANDLE};

VkCommandPool   g_command_pool{VK_NULL_HANDLE};
VkCommandBuffer g_command_buffer{VK_NULL_HANDLE};
VkSemaphore     g_image_available_semaphore{VK_NULL_HANDLE};
VkSemaphore     g_render_finished_semaphore{VK_NULL_HANDLE};
VkFence         g_in_flight_fence{VK_NULL_HANDLE};

#ifndef NDEBUG

char const* kValidationLayer{"VK_LAYER_KHRONOS_validation"};
char const* kDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

VkDebugUtilsMessengerEXT g_debug_messenger{VK_NULL_HANDLE};

bool CheckValidationLayerSupport()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers{layer_count};
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    std::ranges::for_each(available_layers, [](VkLayerProperties const& layer) {
        LogDebug("Available Layer: {}", layer.layerName);
    });

    return std::ranges::any_of(available_layers, [](VkLayerProperties const& layer) {
        return std::strcmp(kValidationLayer, layer.layerName) == 0;
    });
}

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

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& t_create_info)
{
    // clang-format off
    t_create_info = {
        .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext           = nullptr,
        .flags           = 0,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = DebugCallback,
        .pUserData       = nullptr,
    };
    // clang-format on
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance                                t_instance,
    VkDebugUtilsMessengerCreateInfoEXT const* t_create_info,
    VkAllocationCallbacks const*              t_allocator,
    VkDebugUtilsMessengerEXT*                 t_debug_messenger)
{
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(t_instance, "vkCreateDebugUtilsMessengerEXT"));

    if (func != nullptr)
    {
        return func(t_instance, t_create_info, t_allocator, t_debug_messenger);
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

VkResult DestroyDebugUtilsMessengerEXT(
    VkInstance                   t_instance,
    VkDebugUtilsMessengerEXT     t_debug_messenger,
    VkAllocationCallbacks const* t_allocator)
{
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(t_instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (func != nullptr)
    {
        func(t_instance, t_debug_messenger, t_allocator);
        return VK_SUCCESS;
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void SetupDebugUtilsMessenger(VkInstance t_instance)
{
    VkDebugUtilsMessengerCreateInfoEXT create_info;
    PopulateDebugMessengerCreateInfo(create_info);

    if (CreateDebugUtilsMessengerEXT(t_instance, &create_info, nullptr, &g_debug_messenger) != VK_SUCCESS)
    {
        LogWarn("Could not set up a debug messenger!");
    }
}

#endif// !NDEBUG

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

auto FindQueueFamilies(VkPhysicalDevice t_device) -> std::expected<std::uint32_t, bool>
{
    std::uint32_t queue_family_count{0U};
    vkGetPhysicalDeviceQueueFamilyProperties(t_device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families{queue_family_count};
    vkGetPhysicalDeviceQueueFamilyProperties(t_device, &queue_family_count, queue_families.data());

    std::uint32_t queue_index{0U};
    for (; queue_index < queue_families.size(); ++queue_index)
    {
        VkBool32 present_support{false};
        vkGetPhysicalDeviceSurfaceSupportKHR(t_device, queue_index, g_surface, &present_support);

        VkQueueFamilyProperties const& family = queue_families[queue_index];

        // Prefer the device that supports drawing and presentation in the same queue
        if ((family.queueFlags & VK_QUEUE_GRAPHICS_BIT) && present_support)
        {
            break;
        }
    }

    if (queue_index == static_cast<std::uint32_t>(queue_families.size()))
    {
        LogCritical("Graphics Queue family was not found for the Graphics Card!");
        return std::unexpected{false};
    }

    return queue_index;
}

bool CheckDeviceExtensionSupport(VkPhysicalDevice t_device)
{
    std::uint32_t extension_count{0U};
    vkEnumerateDeviceExtensionProperties(t_device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(t_device, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions;
    required_extensions.insert(kDeviceExtensions);

    std::ranges::for_each(available_extensions, [&required_extensions](VkExtensionProperties const& extension) {
        required_extensions.erase(extension.extensionName);
    });

    return required_extensions.empty();
}

auto QuerySwapChainSupport(VkPhysicalDevice t_device) -> std::expected<swap_chain_support_details, bool>
{
    swap_chain_support_details details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(t_device, g_surface, &details.surface_capabilities);

    std::uint32_t surface_format_count{0U};
    vkGetPhysicalDeviceSurfaceFormatsKHR(t_device, g_surface, &surface_format_count, nullptr);

    std::uint32_t present_mode_count{0U};
    vkGetPhysicalDeviceSurfacePresentModesKHR(t_device, g_surface, &present_mode_count, nullptr);

    if (0U == surface_format_count || 0U == present_mode_count)
    {
        return std::unexpected{false};
    }

    details.surface_formats.resize(surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(t_device, g_surface, &surface_format_count, details.surface_formats.data());

    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(t_device, g_surface, &present_mode_count, details.present_modes.data());

    return details;
}

auto ChooseSwapSurfaceFormat(std::span<VkSurfaceFormatKHR> const t_available_formats) -> VkSurfaceFormatKHR
{
    for (auto const& available_format : t_available_formats)
    {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return available_format;
        }
    }

    return t_available_formats[0];
}

auto ChooseSwapPresentMode(std::span<VkPresentModeKHR> const t_available_modes) -> VkPresentModeKHR
{
    for (auto const& available_mode : t_available_modes)
    {
        if (available_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return available_mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

auto ChooseSwapPreset(VkSurfaceCapabilitiesKHR const& t_surface_capabilities) -> VkExtent2D
{
    if (t_surface_capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max())
    {
        return t_surface_capabilities.currentExtent;
    }

    std::int32_t width{0};
    std::int32_t height{0};
    glfwGetFramebufferSize(g_window, &width, &height);

    VkExtent2D actual_extent{
        .width  = static_cast<std::uint32_t>(width),
        .height = static_cast<std::uint32_t>(height),
    };

    actual_extent.width = std::clamp(
        actual_extent.width,
        t_surface_capabilities.minImageExtent.width,
        t_surface_capabilities.maxImageExtent.width);
    actual_extent.height = std::clamp(
        actual_extent.height,
        t_surface_capabilities.minImageExtent.height,
        t_surface_capabilities.maxImageExtent.height);

    return actual_extent;
}

auto CreateShaderModule(std::span<std::byte const> const t_code) -> std::expected<VkShaderModule, VkResult>
{
    VkShaderModuleCreateInfo create_info{
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0U,
        .codeSize = t_code.size(),
        .pCode    = reinterpret_cast<std::uint32_t const*>(t_code.data()),
    };

    VkShaderModule shader_module;
    VkResult       result = vkCreateShaderModule(g_logical_device, &create_info, nullptr, &shader_module);
    if (result != VK_SUCCESS)
    {
        LogError("Could not create a shader module!");
        return std::unexpected{result};
    }

    return shader_module;
}

void RecordCommandBuffer(VkCommandBuffer t_command_buffer, std::uint32_t t_image_index)
{
    VkCommandBufferBeginInfo begin_info{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = 0U,
        .pInheritanceInfo = nullptr,
    };

    if (vkBeginCommandBuffer(t_command_buffer, &begin_info) != VK_SUCCESS)
    {
        LogError("Could not begin recording command buffer!");
        return;
    }

    VkClearValue          clear_color = {.color = {.float32 = {kPolosRed, kPolosGreen, kPolosBlue, 1.0f}}};
    VkRenderPassBeginInfo render_pass_info{
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext           = nullptr,
        .renderPass      = g_render_pass,
        .framebuffer     = g_swap_chain_framebuffers[t_image_index],
        .renderArea      = {.offset = {0, 0}, .extent = g_extent},
        .clearValueCount = 1U,
        .pClearValues    = &clear_color,
    };

    vkCmdBeginRenderPass(t_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(t_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_graphics_pipeline);
    vkCmdSetViewport(t_command_buffer, 0U, 1U, &g_viewport);
    vkCmdSetScissor(t_command_buffer, 0U, 1U, &g_scissor);
    vkCmdDraw(t_command_buffer, 3U, 1U, 0U, 0U);
    vkCmdEndRenderPass(t_command_buffer);

    if (vkEndCommandBuffer(t_command_buffer) != VK_SUCCESS)
    {
        LogError("Could not record command buffer!");
    }
}

}// namespace

auto InitializeVulkan(GLFWwindow* const t_window) -> std::expected<VulkanState, VkResult>
{
    g_window = t_window;
    /// ==========================================
    ///         Initialize vulkan app
    /// ==========================================
    VkApplicationInfo app_info{
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

    std::vector<char const*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

#ifndef NDEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif// !NDEBUG

    std::ranges::for_each(extensions, [count = 0](char const* t_extension) mutable {
        LogDebug("GLFW Extension {}: {}", count++, t_extension);
    });

    std::uint32_t enabled_layer_count{0};
    const char**  enabled_layer_names{nullptr};

#ifndef NDEBUG
    if (!CheckValidationLayerSupport())
    {
        LogWarn("Validation layers are not available");
    }
    enabled_layer_count = 1;
    enabled_layer_names = &kValidationLayer;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    PopulateDebugMessengerCreateInfo(debug_create_info);
#endif// !NDEBUG

    VkInstanceCreateInfo instance_create_info{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#ifndef NDEBUG
        .pNext = &debug_create_info,
#endif// !NDEBUG
        .flags                   = 0U,
        .pApplicationInfo        = &app_info,
        .enabledLayerCount       = enabled_layer_count,
        .ppEnabledLayerNames     = enabled_layer_names,
        .enabledExtensionCount   = static_cast<std::uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),// provides VK_KHR_swapchain
    };

    VkResult result = vkCreateInstance(&instance_create_info, nullptr, &g_instance);
    if (result != VK_SUCCESS)
    {
        LogCritical("Failed to create VkInstance!");
        return std::unexpected(result);
    }

#ifndef NDEBUG
    SetupDebugUtilsMessenger(g_instance);
#endif// !NDEBUG

    /// ==========================================
    ///           Create window surface
    /// ==========================================
    result = glfwCreateWindowSurface(g_instance, t_window, nullptr, &g_surface);
    if (result != VK_SUCCESS)
    {
        LogCritical("Could not create a window surface!");
        return std::unexpected{result};
    }

    /// ==========================================
    ///       Select suitable physical device
    /// ==========================================
    std::uint32_t device_count{0U};
    result = vkEnumeratePhysicalDevices(g_instance, &device_count, nullptr);
    if (0U == device_count)
    {
        return std::unexpected{result};
    }

    // TODO: Move into a free function
    std::uint32_t              queue_index{0U};
    swap_chain_support_details swap_chain_details;

    auto is_device_suitable = [&queue_index, &swap_chain_details](VkPhysicalDevice t_device) {
        VkPhysicalDeviceProperties2 props{
            .sType      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
            .pNext      = nullptr,
            .properties = {},
        };
        vkGetPhysicalDeviceProperties2(t_device, &props);

        LogInfo("Checking suitability for Graphics Device: {}", props.properties.deviceName);

        if (props.properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            LogWarn("Physical device is not a discrete GPU");
            return false;
        }

        auto queue_family_result = FindQueueFamilies(t_device);
        if (!queue_family_result.has_value())
        {
            LogWarn("Could not find a suitable queue family on physical device.");
            return false;
        }

        queue_index = queue_family_result.value();

        if (!CheckDeviceExtensionSupport(t_device))
        {
            return false;
        }

        auto result = QuerySwapChainSupport(t_device);
        if (!result.has_value())
        {
            LogWarn("Required swapchain support was not found for the physical device.");
            return false;
        }

        swap_chain_support_details const& details = result.value();
        if (details.surface_formats.empty() || details.present_modes.empty())
        {
            LogWarn("Swapchain support is inadequate!");
            return false;
        }

        swap_chain_details = details;

        return true;
    };

    /// ==========================================
    ///   Find suitable GPU available on system.
    /// ==========================================
    std::vector<VkPhysicalDevice> devices{device_count};
    vkEnumeratePhysicalDevices(g_instance, &device_count, devices.data());

    auto it = std::ranges::find_if(devices, is_device_suitable);
    if (it == devices.end())
    {
        LogCritical("Could not find any GPU that is suitable!");
        return std::unexpected{VkResult::VK_INCOMPLETE};
    }

    g_phys_device = *it;

    /// ==========================================
    ///   Print out device information
    /// ==========================================
    vkGetPhysicalDeviceProperties2(g_phys_device, &g_device_properties);

    LogInfo(
        "Selected GPU:\n- Device Name: {}\n- Driver Version: {}",
        g_device_properties.properties.deviceName,
        DecodeDriverVersion(g_device_properties.properties.vendorID, g_device_properties.properties.driverVersion));

    constexpr float const queue_prio{1.0f};

    /// ==========================================
    ///           Create Device Queues
    /// ==========================================
    VkDeviceQueueCreateInfo queue_create_info{
        .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = 0U,
        .queueFamilyIndex = queue_index,
        .queueCount       = 1U,
        .pQueuePriorities = &queue_prio,
    };

    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(g_phys_device, &device_features);

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
        .enabledLayerCount   = enabled_layer_count,
        .ppEnabledLayerNames = enabled_layer_names,
        // ----------------------------------------------------------------------
        .enabledExtensionCount   = 1U,
        .ppEnabledExtensionNames = &kDeviceExtensions,
        .pEnabledFeatures        = &device_features,
    };

    result = vkCreateDevice(g_phys_device, &device_create_info, nullptr, &g_logical_device);
    if (result != VK_SUCCESS)
    {
        LogCritical("Logical device creation for GPU: {} has failed!", g_device_properties.properties.deviceName);
        return std::unexpected{result};
    }

    vkGetDeviceQueue(g_logical_device, queue_index, 0, &g_gfx_present_queue);
    LogInfo("Graphics Queue Index: {}", queue_index);

    /// ==========================================
    ///         Create swap chain
    /// ==========================================
    VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_details.surface_formats);
    VkPresentModeKHR   present_mode   = ChooseSwapPresentMode(swap_chain_details.present_modes);
    g_extent                          = ChooseSwapPreset(swap_chain_details.surface_capabilities);

    g_scissor = {
        .offset = {0, 0},
        .extent = g_extent,
    };
    g_viewport = {
        .x        = 0.0f,
        .y        = 0.0f,
        .width    = static_cast<float>(g_extent.width),
        .height   = static_cast<float>(g_extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    std::uint32_t image_count = swap_chain_details.surface_capabilities.minImageCount + 1U;
    if (swap_chain_details.surface_capabilities.maxImageCount > 0U &&
        image_count > swap_chain_details.surface_capabilities.maxImageCount)
    {
        image_count = swap_chain_details.surface_capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swap_chain_create_info{
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .surface               = g_surface,
        .minImageCount         = image_count,
        .imageFormat           = surface_format.format,
        .imageColorSpace       = surface_format.colorSpace,
        .imageExtent           = g_extent,
        .imageArrayLayers      = 1U,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0U,
        .pQueueFamilyIndices   = nullptr,
        .preTransform          = swap_chain_details.surface_capabilities.currentTransform,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = present_mode,
        .clipped               = VK_TRUE,
        .oldSwapchain          = VK_NULL_HANDLE,
    };

    result = vkCreateSwapchainKHR(g_logical_device, &swap_chain_create_info, nullptr, &g_swap_chain);
    if (result != VK_SUCCESS)
    {
        LogCritical("Could not create a swapchain for the window surface!");
        return std::unexpected{result};
    }

    /// ==========================================
    ///      Create image views for swap chain
    /// ==========================================
    std::vector<VkImage> swap_chain_images;
    std::uint32_t        swap_chain_image_count{0U};
    {
        vkGetSwapchainImagesKHR(g_logical_device, g_swap_chain, &swap_chain_image_count, nullptr);

        swap_chain_images.resize(swap_chain_image_count);
        vkGetSwapchainImagesKHR(g_logical_device, g_swap_chain, &swap_chain_image_count, swap_chain_images.data());

        LogInfo("Number of images in the swap chain: {}", swap_chain_image_count);
    }

    g_swap_chain_image_views.resize(static_cast<std::size_t>(swap_chain_image_count));
    for (std::size_t i = 0U; i < static_cast<std::size_t>(swap_chain_image_count); ++i)
    {
        VkImageViewCreateInfo create_info{
            .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext    = nullptr,
            .flags    = 0U,
            .image    = swap_chain_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format   = surface_format.format,
            .components{
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange{
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0U,
                .levelCount     = 1U,
                .baseArrayLayer = 0U,
                .layerCount     = 1U,
            },
        };

        result = vkCreateImageView(g_logical_device, &create_info, nullptr, &g_swap_chain_image_views[i]);
        if (result != VK_SUCCESS)
        {
            LogCritical("Could not create image views for the swap chain images!");
            return std::unexpected{result};
        }
    }

    VkAttachmentDescription2 color_attachment{
        .sType          = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
        .pNext          = nullptr,
        .flags          = 0U,
        .format         = surface_format.format,
        .samples        = VK_SAMPLE_COUNT_1_BIT,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    /// ==========================================
    ///          Create shader modules
    /// ==========================================
    auto vertex_shader_code   = fs::ReadFile(std::filesystem::path("Shaders/basic_color.vert.spv"));
    auto fragment_shader_code = fs::ReadFile(std::filesystem::path("Shaders/basic_color.frag.spv"));
    if (!vertex_shader_code.has_value() || !fragment_shader_code.has_value())
    {
        LogCritical("Could not read shader files!");
        return std::unexpected{VkResult::VK_INCOMPLETE};
    }

    auto vertex_shader_module_result = CreateShaderModule(vertex_shader_code.value().data);
    if (!vertex_shader_module_result.has_value())
    {
        LogError("Could not create vertex shader module!");
        return std::unexpected{vertex_shader_module_result.error()};
    }
    g_vertex_shader_module = vertex_shader_module_result.value();

    auto fragment_shader_module_result = CreateShaderModule(fragment_shader_code.value().data);
    if (!fragment_shader_module_result.has_value())
    {
        LogError("Could not create fragment shader module!");
        return std::unexpected{fragment_shader_module_result.error()};
    }
    g_fragment_shader_module = fragment_shader_module_result.value();


    /// ==========================================
    ///       Create graphics pipeline
    /// ==========================================
    VkPipelineShaderStageCreateInfo shader_stages[2]{
        {
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0U,
            .stage               = VK_SHADER_STAGE_VERTEX_BIT,
            .module              = g_vertex_shader_module,
            .pName               = "main",
            .pSpecializationInfo = nullptr,
        },
        {
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0U,
            .stage               = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module              = g_fragment_shader_module,
            .pName               = "main",
            .pSpecializationInfo = nullptr,
        },
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_info{
        .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext                           = nullptr,
        .flags                           = 0U,
        .vertexBindingDescriptionCount   = 0U,
        .pVertexBindingDescriptions      = nullptr,
        .vertexAttributeDescriptionCount = 0U,
        .pVertexAttributeDescriptions    = nullptr,
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0U,
        .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkPipelineViewportStateCreateInfo viewport_state{
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0U,
        .viewportCount = 1U,
        .pViewports    = &g_viewport,
        .scissorCount  = 1U,
        .pScissors     = &g_scissor,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = VK_POLYGON_MODE_FILL,
        .cullMode                = VK_CULL_MODE_BACK_BIT,
        .frontFace               = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth               = 1.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisampling{
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable   = VK_FALSE,
        .minSampleShading      = 1.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };

    // clang-format off
    VkPipelineColorBlendAttachmentState color_blend_attachment{
        .blendEnable         = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp        = VK_BLEND_OP_ADD,
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT,
    };
    // clang-format on

    VkPipelineColorBlendStateCreateInfo color_blending{
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0U,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_COPY,
        .attachmentCount = 1U,
        .pAttachments    = &color_blend_attachment,
        .blendConstants  = {0.0f, 0.0f, 0.0f, 0.0f},
    };

    VkPipelineLayoutCreateInfo pipeline_layout_info{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0U,
        .setLayoutCount         = 0U,
        .pSetLayouts            = nullptr,
        .pushConstantRangeCount = 0U,
        .pPushConstantRanges    = nullptr,
    };

    result = vkCreatePipelineLayout(g_logical_device, &pipeline_layout_info, nullptr, &g_pipeline_layout);
    if (result != VK_SUCCESS)
    {
        LogCritical("Could not create a pipeline layout!");
        return std::unexpected{result};
    }

    VkPipelineRenderingCreateInfo pipeline_rendering_info{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext                   = nullptr,
        .viewMask                = 0U,
        .colorAttachmentCount    = 1U,
        .pColorAttachmentFormats = &surface_format.format,
        .depthAttachmentFormat   = VK_FORMAT_UNDEFINED,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    std::vector<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state{
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = 0U,
        .dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size()),
        .pDynamicStates    = dynamic_states.data(),
    };

    // Create attachment reference for the color attachment
    VkAttachmentReference2 color_attachment_ref{
        .sType      = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
        .pNext      = nullptr,
        .attachment = 0U,
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    };

    VkSubpassDescription2 subpass{
        .sType                   = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .viewMask                = 0U,
        .inputAttachmentCount    = 0U,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = 1U,
        .pColorAttachments       = &color_attachment_ref,
        .pResolveAttachments     = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0U,
        .pPreserveAttachments    = nullptr,
    };

    VkSubpassDependency2 dependency{
        .sType           = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
        .pNext           = nullptr,
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0U,
        .srcStageMask    = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask    = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask   = 0U,
        .dstAccessMask   = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0U,
        .viewOffset      = 0U,
    };

    VkRenderPassCreateInfo2 render_pass_info{
        .sType                   = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .attachmentCount         = 1U,
        .pAttachments            = &color_attachment,
        .subpassCount            = 1U,
        .pSubpasses              = &subpass,
        .dependencyCount         = 1U,
        .pDependencies           = &dependency,
        .correlatedViewMaskCount = 0U,
        .pCorrelatedViewMasks    = nullptr,
    };

    result = vkCreateRenderPass2(g_logical_device, &render_pass_info, nullptr, &g_render_pass);
    if (result != VK_SUCCESS)
    {
        LogCritical("Could not create a render pass!");
        return std::unexpected{result};
    }

    VkGraphicsPipelineCreateInfo pipeline_info{
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = &pipeline_rendering_info,
        .flags               = 0U,
        .stageCount          = 2U,
        .pStages             = shader_stages,
        .pVertexInputState   = &vertex_input_info,
        .pInputAssemblyState = &input_assembly,
        .pTessellationState  = nullptr,
        .pViewportState      = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        .pDepthStencilState  = nullptr,
        .pColorBlendState    = &color_blending,
        .pDynamicState       = &dynamic_state,
        .layout              = g_pipeline_layout,
        .renderPass          = g_render_pass,
        .subpass             = 0U,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = -1,
    };

    result =
        vkCreateGraphicsPipelines(g_logical_device, VK_NULL_HANDLE, 1U, &pipeline_info, nullptr, &g_graphics_pipeline);
    if (result != VK_SUCCESS)
    {
        LogCritical("Could not create a graphics pipeline!");
        return std::unexpected{result};
    }

    g_swap_chain_framebuffers.resize(swap_chain_image_count);
    for (std::size_t i = 0U; i < swap_chain_image_count; ++i)
    {
        VkImageView attachments[] = {
            g_swap_chain_image_views[i],
        };

        VkFramebufferCreateInfo framebuffer_info{
            .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext           = nullptr,
            .flags           = 0U,
            .renderPass      = g_render_pass,
            .attachmentCount = 1U,
            .pAttachments    = attachments,
            .width           = g_extent.width,
            .height          = g_extent.height,
            .layers          = 1U,
        };

        result = vkCreateFramebuffer(g_logical_device, &framebuffer_info, nullptr, &g_swap_chain_framebuffers[i]);
        if (result != VK_SUCCESS)
        {
            LogCritical("Could not create framebuffer {} for the swap chain image view!", i);
            return std::unexpected{result};
        }
    }

    VkCommandPoolCreateInfo pool_info{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queue_index,
    };

    result = vkCreateCommandPool(g_logical_device, &pool_info, nullptr, &g_command_pool);
    if (result != VK_SUCCESS)
    {
        LogCritical("Could not create a command pool!");
        return std::unexpected{result};
    }

    VkCommandBufferAllocateInfo alloc_info{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = g_command_pool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1U,
    };

    result = vkAllocateCommandBuffers(g_logical_device, &alloc_info, &g_command_buffer);
    if (result != VK_SUCCESS)
    {
        LogCritical("Could not allocate command buffers!");
        return std::unexpected{result};
    }

    VkSemaphoreCreateInfo semaphore_info{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
    };
    result = vkCreateSemaphore(g_logical_device, &semaphore_info, nullptr, &g_image_available_semaphore);
    if (result != VK_SUCCESS)
    {
        LogCritical("Could not create image available semaphore!");
        return std::unexpected{result};
    }

    result = vkCreateSemaphore(g_logical_device, &semaphore_info, nullptr, &g_render_finished_semaphore);
    if (result != VK_SUCCESS)
    {
        LogCritical("Could not create render finished semaphore!");
        return std::unexpected{result};
    }

    VkFenceCreateInfo fence_info{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    result = vkCreateFence(g_logical_device, &fence_info, nullptr, &g_in_flight_fence);
    if (result != VK_SUCCESS)
    {
        LogCritical("Could not create in-flight fence!");
        return std::unexpected{result};
    }

    /// ==========================================
    ///          Return final variables
    /// ==========================================
    return VulkanState{
        .instance   = g_instance,
        .device     = g_logical_device,
        .gfx_queue  = g_gfx_present_queue,
        .surface    = g_surface,
        .swap_chain = g_swap_chain,
    };
}

void TerminateVulkan()
{
    // Wait until the device is idle before destroying resources
    // This ensures that no commands are being executed while we are destroying resources
    vkDeviceWaitIdle(g_logical_device);

    vkDestroySemaphore(g_logical_device, g_render_finished_semaphore, nullptr);
    vkDestroySemaphore(g_logical_device, g_image_available_semaphore, nullptr);
    vkDestroyFence(g_logical_device, g_in_flight_fence, nullptr);
    vkDestroyCommandPool(g_logical_device, g_command_pool, nullptr);

    for (auto framebuffer : g_swap_chain_framebuffers) { vkDestroyFramebuffer(g_logical_device, framebuffer, nullptr); }
    vkDestroyPipeline(g_logical_device, g_graphics_pipeline, nullptr);
    vkDestroyRenderPass(g_logical_device, g_render_pass, nullptr);
    vkDestroyPipelineLayout(g_logical_device, g_pipeline_layout, nullptr);
    vkDestroyShaderModule(g_logical_device, g_fragment_shader_module, nullptr);
    vkDestroyShaderModule(g_logical_device, g_vertex_shader_module, nullptr);
    for (auto image_view : g_swap_chain_image_views) { vkDestroyImageView(g_logical_device, image_view, nullptr); }
    vkDestroySwapchainKHR(g_logical_device, g_swap_chain, nullptr);

    vkDestroyDevice(g_logical_device, nullptr);
#ifndef NDEBUG
    DestroyDebugUtilsMessengerEXT(g_instance, g_debug_messenger, nullptr);
#endif// !NDEBUG
    vkDestroySurfaceKHR(g_instance, g_surface, nullptr);
    vkDestroyInstance(g_instance, nullptr);
}

void RenderFrame()
{
    vkWaitForFences(g_logical_device, 1U, &g_in_flight_fence, VK_TRUE, std::numeric_limits<std::uint64_t>::max());
    vkResetFences(g_logical_device, 1U, &g_in_flight_fence);

    std::uint32_t image_index{0U};
    VkResult      result = vkAcquireNextImageKHR(
        g_logical_device,
        g_swap_chain,
        std::numeric_limits<std::uint64_t>::max(),
        g_image_available_semaphore,
        VK_NULL_HANDLE,
        &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        LogWarn("Swap chain is out of date!");
        // RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        LogError("Failed to acquire swap chain image!");
        return;
    }

    vkResetCommandBuffer(g_command_buffer, 0U);
    RecordCommandBuffer(g_command_buffer, image_index);

    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submit_info{
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = 1U,
        .pWaitSemaphores      = &g_image_available_semaphore,
        .pWaitDstStageMask    = wait_stages,
        .commandBufferCount   = 1U,
        .pCommandBuffers      = &g_command_buffer,
        .signalSemaphoreCount = 1U,
        .pSignalSemaphores    = &g_render_finished_semaphore,
    };

    result = vkQueueSubmit(g_gfx_present_queue, 1U, &submit_info, g_in_flight_fence);
    if (result != VK_SUCCESS)
    {
        LogError("Could not submit draw command buffer to the graphics queue!");
        return;
    }

    VkPresentInfoKHR present_info{
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext              = nullptr,
        .waitSemaphoreCount = 1U,
        .pWaitSemaphores    = &g_render_finished_semaphore,
        .swapchainCount     = 1U,
        .pSwapchains        = &g_swap_chain,
        .pImageIndices      = &image_index,
        .pResults           = nullptr,
    };

    result = vkQueuePresentKHR(g_gfx_present_queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        LogWarn("Swap chain is out of date or suboptimal!");
        // RecreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        LogError("Failed to present swap chain image!");
    }

    return;
}

}// namespace polos::rendering
