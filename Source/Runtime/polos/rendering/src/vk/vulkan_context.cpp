//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "vk/vulkan_context.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "vk/common.hpp"
#include "vk/settings.hpp"

#include <algorithm>
#include <string>

namespace polos::rendering
{

namespace
{

constexpr char const* kValidationLayerName = "VK_LAYER_KHRONOS_validation";

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      tMessageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             tMessageType,
    VkDebugUtilsMessengerCallbackDataEXT const* tCallbackData,
    void* /*tUserData*/)
{
    std::string const message_type_str = [&]() {
        switch (tMessageType)
        {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: return "GENERAL";
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: return "VALIDATION";
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: return "PERFORMANCE";
            default: return "UNKNOWN";
        }
    }();

    switch (tMessageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LogWarn("Vulkan {}: {}", message_type_str, tCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LogError("Vulkan {}: {}", message_type_str, tCallbackData->pMessage);
            break;
        default: break;
    }

    return VK_FALSE;
}

auto CreateDebugMessengerCreateInfo() -> VkDebugUtilsMessengerCreateInfoEXT
{
    // clang-format off
    return VkDebugUtilsMessengerCreateInfoEXT{
        .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext           = nullptr,
        .flags           = 0U,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType     = static_cast<VkDebugUtilsMessageTypeFlagsEXT>(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) |
                           static_cast<VkDebugUtilsMessageTypeFlagsEXT>(VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) |
                           static_cast<VkDebugUtilsMessageTypeFlagsEXT>(VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT),
        .pfnUserCallback = DebugCallback,
        .pUserData       = nullptr,
    };
    // clang-format on
}

bool IsValidationLayersAvailable()
{
    std::uint32_t layer_count{0U};
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers{layer_count};
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    std::ranges::for_each(available_layers, [](VkLayerProperties const& tLayer) {
        LogDebug("Available Layer: {}", tLayer.layerName);
    });

    return std::ranges::any_of(available_layers, [](VkLayerProperties const& tLayer) {
        std::span<char const> const layer_name(tLayer.layerName);
        return std::strcmp(kValidationLayerName, layer_name.data()) == 0;
    });
}

}// namespace

VulkanContext::VulkanContext()  = default;
VulkanContext::~VulkanContext() = default;

auto VulkanContext::Create(ContextCreateDetails const& tDetails) -> Result<void>
{
    VkApplicationInfo const app{
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext              = nullptr,
        .pApplicationName   = "Polos App",
        .applicationVersion = VK_API_VERSION_1_3,
        .pEngineName        = "Polos",
        .engineVersion      = VK_API_VERSION_1_3,
        .apiVersion         = VK_API_VERSION_1_3,
    };

    mExtensions = tDetails.requiredExtensions;

    VkDebugUtilsMessengerCreateInfoEXT debug_info{};
    if (Settings::kEnableValidationLayers)
    {
        LogDebug("Creating Vulkan Instance with Debug Messenger");
        enableValidationLayers();
        LogDebug("Enabled validation layers.");

        debug_info = CreateDebugMessengerCreateInfo();
    }
    VkInstanceCreateInfo const instance_info{
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext                   = Settings::kEnableValidationLayers ? &debug_info : nullptr,
        .flags                   = 0U,
        .pApplicationInfo        = &app,
        .enabledLayerCount       = VK_SIZE_CAST(mLayers.size()),
        .ppEnabledLayerNames     = mLayers.data(),
        .enabledExtensionCount   = VK_SIZE_CAST(mExtensions.size()),
        .ppEnabledExtensionNames = mExtensions.data(),// provides VK_KHR_swapchain
    };

    CHECK_VK_SUCCESS_OR_ERR(
        vkCreateInstance(&instance_info, nullptr, &mInstance),
        RenderingErrc::kFailedCreateInstance);

    if (Settings::kEnableValidationLayers)
    {
        VkDebugUtilsMessengerCreateInfoEXT const debug_info_2 = CreateDebugMessengerCreateInfo();
        auto const                               func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(//NOLINT
            vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT"));
        if (nullptr != func)
        {
            func(mInstance, &debug_info_2, nullptr, &mDbgMessenger);
        }
        else
        {
            LogWarn("vkCreateDebugUtilsMessengerEXT function was not found!");
        }
    }
    return {};
}

auto VulkanContext::Destroy() -> Result<void>
{
    if (Settings::kEnableValidationLayers)
    {
        auto const func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(// NOLINT
            vkGetInstanceProcAddr(mInstance, "vkDestroyDebugUtilsMessengerEXT"));

        if (func != nullptr && mDbgMessenger != VK_NULL_HANDLE)
        {
            func(mInstance, mDbgMessenger, nullptr);
        }
    }
    vkDestroyInstance(mInstance, nullptr);

    LogInfo("Destoyed VulkanContext and DebugMessenger");

    return {};
}

auto VulkanContext::enableValidationLayers() -> void
{
    // No need for enabling debug utils extension if validation layers are not available anyways.
    if (!IsValidationLayersAvailable())
    {
        LogWarn("Validation layers are not available at this time.");
        return;
    }

    mExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    mLayers.push_back("VK_LAYER_KHRONOS_validation");
}

}// namespace polos::rendering
