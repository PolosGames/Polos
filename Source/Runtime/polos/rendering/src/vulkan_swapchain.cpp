//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vulkan_swapchain.hpp"

#include "polos/communication/error_code.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/vulkan_device.hpp"
#include "polos/rendering/vulkan_resource_manager.hpp"

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdint>

namespace polos::rendering
{

VulkanSwapchain::VulkanSwapchain(GLFWwindow* t_window)
    : m_window{t_window}
{}

VulkanSwapchain::~VulkanSwapchain() = default;

auto VulkanSwapchain::Create(swapchain_create_details const& t_details) -> Result<void>
{
    m_surface   = t_details.surface;
    m_gfx_queue = t_details.gfx_queue;
    m_device    = t_details.device->logi_device;

    setupExtentAndViewport(t_details.phys_device);

    if (!selectFormatAndMode(t_details.device, t_details.preferred_surface_formats, t_details.preferred_present_modes))
    {
        return ErrorType{RenderingErrc::kNoAdequateSurface};
    }

    if (auto res = createSwapchainHandle(t_details.transform_flags); !res.has_value())
    {
        return ErrorType{res.error()};
    }

    LogInfo("Swapchain object created, receiving images for it...");

    if (auto res = createImageViews(); !res.has_value())
    {
        return ErrorType{res.error()};
    }

    LogInfo("Swapchain creation done!");

    return {};
}

auto VulkanSwapchain::setupExtentAndViewport(VkPhysicalDevice t_phys_device) -> void
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(t_phys_device, m_surface, &m_surface_cap);

    std::int32_t width{0U};
    std::int32_t height{0U};
    glfwGetFramebufferSize(m_window, &width, &height);

    m_extent.width = std::clamp(
        static_cast<std::uint32_t>(width),
        m_surface_cap.minImageExtent.width,
        m_surface_cap.maxImageExtent.width);
    m_extent.height = std::clamp(
        static_cast<std::uint32_t>(height),
        m_surface_cap.minImageExtent.height,
        m_surface_cap.maxImageExtent.height);

    m_scissor.offset = {.x = 0, .y = 0};
    m_scissor.extent = m_extent;

    m_viewport.x        = 0.0F;
    m_viewport.y        = 0.0F;
    m_viewport.width    = static_cast<float>(m_extent.width);
    m_viewport.height   = static_cast<float>(m_extent.height);
    m_viewport.minDepth = 0.0F;
    m_viewport.maxDepth = 1.0F;

    m_img_count = m_surface_cap.minImageCount + 1U;
    // No need to have more images then the surface supports for now.
    if (m_surface_cap.maxImageCount > 0U && m_img_count > m_surface_cap.maxImageCount)
    {
        m_img_count = m_surface_cap.maxImageCount;
    }
}

auto VulkanSwapchain::selectFormatAndMode(
    VulkanDevice const*                    t_device,
    std::vector<VkSurfaceFormatKHR> const& t_formats,
    std::vector<VkPresentModeKHR> const&   t_modes) -> bool
{
    bool is_surface_adequate{false};
    for (auto const& format : t_formats)
    {
        if (t_device->CheckSurfaceFormatSupport(format))
        {
            m_surface_format    = format;
            is_surface_adequate = true;
        }
    }
    for (auto const mode : t_modes)
    {
        if (t_device->CheckPresentModeSupport(mode))
        {
            m_present_mode      = mode;
            is_surface_adequate = true;
        }
    }
    return is_surface_adequate;
}

auto VulkanSwapchain::createSwapchainHandle(VkSurfaceTransformFlagsKHR t_transform_flags) -> Result<void>
{
    VkSwapchainCreateInfoKHR info{
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .surface               = m_surface,
        .minImageCount         = m_img_count,
        .imageFormat           = m_surface_format.format,
        .imageColorSpace       = m_surface_format.colorSpace,
        .imageExtent           = m_extent,
        .imageArrayLayers      = 1U,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0U,
        .pQueueFamilyIndices   = nullptr,
        .preTransform          = static_cast<VkSurfaceTransformFlagBitsKHR>(t_transform_flags),
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = m_present_mode,
        .clipped               = VK_TRUE,
        .oldSwapchain          = VK_NULL_HANDLE,
    };

    // Enable transfer source on swap chain images if supported
    if (0U != (m_surface_cap.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT))
    {
        info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    // Enable transfer destination on swap chain images if supported
    if (0U != (m_surface_cap.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
    {
        info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    CHECK_VK_SUCCESS_OR_ERR(
        vkCreateSwapchainKHR(m_device, &info, nullptr, &m_swapchain),
        RenderingErrc::kFailedCreateSwapchain);

    return {};
}

auto VulkanSwapchain::createImageViews() -> Result<void>
{
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_img_count, nullptr);
    m_images.resize(static_cast<std::size_t>(m_img_count));
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_img_count, m_images.data());
    m_image_views.resize(static_cast<std::size_t>(m_img_count));

    LogInfo("Received {} swapchain images.", m_img_count);

    auto const img_count = static_cast<std::size_t>(m_img_count);

    for (std::size_t i = 0U; i < img_count; ++i)
    {
        VkImageViewCreateInfo const create_info{
            .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext    = nullptr,
            .flags    = 0U,
            .image    = m_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format   = m_surface_format.format,
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

        CHECK_VK_SUCCESS_OR_ERR(
            vkCreateImageView(m_device, &create_info, nullptr, &m_image_views[i]),
            RenderingErrc::kFailedCreateSwapchainImageViews);
    }

    LogInfo("Created {} image views.", m_image_views.size());

    return {};
}

auto VulkanSwapchain::Destroy() -> Result<void>
{
    LogInfo("Destroying VulkanSwapchain and associated images...");

    std::ranges::for_each(m_image_views, [this](VkImageView& t_view) {
        vkDestroyImageView(m_device, t_view, nullptr);
    });

    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

    m_image_views.clear();
    m_images.clear();

    return {};
}

auto VulkanSwapchain::GetSurfaceFormat() const -> VkSurfaceFormatKHR const&
{
    return m_surface_format;
}

auto VulkanSwapchain::GetExtent() const -> VkExtent2D const&
{
    return m_extent;
}

auto VulkanSwapchain::GetScissor() const -> VkRect2D const&
{
    return m_scissor;
}

auto VulkanSwapchain::GetViewport() const -> VkViewport const&
{
    return m_viewport;
}

auto VulkanSwapchain::AcquireNextImage(acquire_next_image_details const& t_details) -> Result<std::uint32_t>
{
    VkResult const res = vkAcquireNextImageKHR(
        m_device,
        m_swapchain,
        t_details.timeout,
        t_details.semaphore,
        VK_NULL_HANDLE,
        &m_current_image);

    if (VK_ERROR_OUT_OF_DATE_KHR == res)
    {
        LogError("Swapchain image is out of date.");
    }

    CHECK_VK_SUCCESS_OR_ERR(res, RenderingErrc::kFailedAcquireNextImage);

    return m_current_image;
}

auto VulkanSwapchain::QueuePresent(VkSemaphore t_wait_semaphore) const -> Result<void>
{
    VkPresentInfoKHR const present_info{
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext              = nullptr,
        .waitSemaphoreCount = 1U,
        .pWaitSemaphores    = &t_wait_semaphore,
        .swapchainCount     = 1U,
        .pSwapchains        = &m_swapchain,
        .pImageIndices      = &m_current_image,
        .pResults           = nullptr,
    };

    VkResult const res = vkQueuePresentKHR(m_gfx_queue, &present_info);

    if (VK_ERROR_OUT_OF_DATE_KHR == res)
    {
        LogError("Swapchain image is out of date.");
    }

    CHECK_VK_SUCCESS_OR_ERR(res, RenderingErrc::kFailedPresentQueue);

    return {};
}

auto VulkanSwapchain::GetCurrentImage() const -> VkImage
{
    return GetImage(m_current_image);
}

auto VulkanSwapchain::GetCurrentImageIndex() const -> std::uint32_t
{
    return m_current_image;
}

auto VulkanSwapchain::GetCurrentImageView() const -> VkImageView
{
    return GetImageView(m_current_image);
}

auto VulkanSwapchain::GetImage(std::uint32_t t_index) const -> VkImage
{
    return m_images[static_cast<std::size_t>(t_index)];
}

auto VulkanSwapchain::GetImageView(std::uint32_t t_index) const -> VkImageView
{
    return m_image_views[static_cast<std::size_t>(t_index)];
}

auto VulkanSwapchain::GetImageCount() const -> std::uint32_t
{
    return m_img_count;
}

}// namespace polos::rendering
