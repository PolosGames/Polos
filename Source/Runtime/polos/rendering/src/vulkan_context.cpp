//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vulkan_context.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"

#include <algorithm>
#include <string>

namespace polos::rendering
{

namespace
{
constexpr char const* kValidationLayerName = "VK_LAYER_KHRONOS_validation";

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

}// namespace

VulkanContext::VulkanContext()  = default;
VulkanContext::~VulkanContext() = default;

auto VulkanContext::Create() -> Result<void>
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
        .enabledLayerCount       = VK_SIZE_CAST(m_layers.size()),
        .ppEnabledLayerNames     = m_layers.data(),
        .enabledExtensionCount   = VK_SIZE_CAST(m_extensions.size()),
        .ppEnabledExtensionNames = m_extensions.data(),// provides VK_KHR_swapchain
    };

    CHECK_VK_SUCCESS_OR_ERR(vkCreateInstance(&instance_info, nullptr, &instance), RenderingErrc::kInstanceNotCreated);

#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT debug_info_2 = create_debug_messenger_create_info();
    auto                               func         = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (nullptr != func)
    {
        func(instance, &debug_info_2, nullptr, &m_dbg_messenger);
    }
    else
    {
        LogWarn("vkCreateDebugUtilsMessengerEXT function was not found!");
    }
#endif// NDEBUG

    return {};
}

auto VulkanContext::Destroy() -> Result<void>
{
#ifndef NDEBUG
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (func != nullptr && m_dbg_messenger != VK_NULL_HANDLE)
    {
        func(instance, m_dbg_messenger, nullptr);
    }
#endif// !NDEBUG
    vkDestroyInstance(instance, nullptr);

    return {};
}

auto VulkanContext::enable_validation_layers() -> void
{
    // No need for enabling debug utils extension if validation layers are not available anyways.
    if (!IsValidationLayersAvailable())
    {
        LogWarn("Validation layers are not available at this time.");
        return;
    }

    m_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    m_layers.push_back("VK_LAYER_KHRONOS_validation");
}

VkDebugUtilsMessengerCreateInfoEXT VulkanContext::create_debug_messenger_create_info() const
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
