//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vulkan_swapchain.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/vulkan_context.hpp"
#include "polos/rendering/vulkan_device.hpp"
#include "polos/rendering/vulkan_resource_manager.hpp"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdint>
#include <limits>

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

    VkSurfaceCapabilitiesKHR cap;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(t_details.phys_device, m_surface, &cap);

    std::int32_t width{0U};
    std::int32_t height{0U};
    glfwGetFramebufferSize(m_window, &width, &height);

    m_extent.width = std::clamp(static_cast<std::uint32_t>(width), cap.minImageExtent.width, cap.maxImageExtent.width);
    m_extent.height =
        std::clamp(static_cast<std::uint32_t>(height), cap.minImageExtent.height, cap.maxImageExtent.height);

    m_scissor.offset = {0, 0};
    m_scissor.extent = m_extent;

    m_viewport.x        = 0.0f;
    m_viewport.y        = 0.0f;
    m_viewport.width    = static_cast<float>(m_extent.width);
    m_viewport.height   = static_cast<float>(m_extent.height);
    m_viewport.minDepth = 0.0f;
    m_viewport.maxDepth = 1.0f;

    m_img_count = cap.minImageCount + 1U;
    // No need to have more images then the surface supports for now.
    if (cap.maxImageCount > 0U && m_img_count > cap.maxImageCount)
    {
        m_img_count = cap.maxImageCount;
    }

    m_device = t_details.device->logi_device;

    bool is_surface_adequate{false};
    for (auto const& format : t_details.preferred_surface_formats)
    {
        if (t_details.device->CheckSurfaceFormatSupport(format))
        {
            m_surface_format    = format;
            is_surface_adequate = true;
        }
    }
    for (auto const mode : t_details.preferred_present_modes)
    {
        if (t_details.device->CheckPresentModeSupport(mode))
        {
            m_present_mode      = mode;
            is_surface_adequate = true;
        }
    }

    if (!is_surface_adequate)
    {
        return ErrorType{RenderingErrc::kNoAdequateSurface};
    }

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
        .preTransform          = static_cast<VkSurfaceTransformFlagBitsKHR>(t_details.transform_flags),
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = m_present_mode,
        .clipped               = VK_TRUE,
        .oldSwapchain          = VK_NULL_HANDLE,
    };

    // Enable transfer source on swap chain images if supported
    if (cap.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
    {
        info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    // Enable transfer destination on swap chain images if supported
    if (cap.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    {
        info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    CHECK_VK_SUCCESS_OR_ERR(
        vkCreateSwapchainKHR(m_device, &info, nullptr, &m_swapchain),
        RenderingErrc::kSwapchainNotCreated);

    vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_img_count, nullptr);
    m_images.resize(static_cast<std::size_t>(m_img_count));
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_img_count, m_images.data());
    m_image_views.resize(static_cast<std::size_t>(m_img_count));

    for (std::size_t i = 0U; i < static_cast<std::size_t>(m_img_count); ++i)
    {
        VkImageViewCreateInfo create_info{
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
            vkCreateImageView(t_details.device->logi_device, &create_info, nullptr, &m_image_views[i]),
            RenderingErrc::kSwapchainImageViewCreationFail);
    }

    return {};
}

auto VulkanSwapchain::Destroy() -> Result<void>
{
    LogInfo("Destroying VulkanSwapchain and associated images...");

    for (auto const& view : m_image_views) { vkDestroyImageView(m_device, view, nullptr); }
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

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
    CHECK_VK_SUCCESS_OR_ERR(
        vkAcquireNextImageKHR(
            m_device,
            m_swapchain,
            t_details.timeout,
            t_details.semaphore,
            VK_NULL_HANDLE,
            &m_current_image),
        RenderingErrc::kFailedToAcquireSwapchainImage);

    return m_current_image;
}

auto VulkanSwapchain::QueuePresent(VkSemaphore t_wait_semaphore) const -> Result<void>
{
    VkPresentInfoKHR present_info{
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext              = nullptr,
        .waitSemaphoreCount = 1U,
        .pWaitSemaphores    = &t_wait_semaphore,
        .swapchainCount     = 1U,
        .pSwapchains        = &m_swapchain,
        .pImageIndices      = &m_current_image,
        .pResults           = nullptr,
    };

    VkResult res = vkQueuePresentKHR(m_gfx_queue, &present_info);

    if (VK_ERROR_OUT_OF_DATE_KHR == res || VK_SUBOPTIMAL_KHR == res)
    {
        LogError("Swapchain image is out of date.");
    }

    CHECK_VK_SUCCESS_OR_ERR(res, RenderingErrc::kFailedToPresentQueue);

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
