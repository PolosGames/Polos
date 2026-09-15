//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "vk/vulkan_swapchain.hpp"

#include "polos/communication/error_code.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "vk/common.hpp"
#include "vk/vulkan_device.hpp"
#include "vk/vulkan_resource_manager.hpp"

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cstdint>

namespace polos::rendering
{

VulkanSwapchain::VulkanSwapchain() = default;

VulkanSwapchain::~VulkanSwapchain() = default;

auto VulkanSwapchain::Create(SwapchainCreateDetails const& tDetails) -> Result<void>
{
    mSurface       = tDetails.surface;
    mGfxQueue      = tDetails.gfxQueue;
    mDevice        = tDetails.device->mLogiDevice;
    mWindowSurface = tDetails.windowSurface;

    setupExtentAndViewport(tDetails.physDevice);

    if (!selectFormatAndMode(tDetails.device, tDetails.preferredSurfaceFormats, tDetails.preferredPresentModes))
    {
        return ErrorType{RenderingErrc::kNoAdequateSurface};
    }

    if (auto res = createSwapchainHandle(tDetails.transformFlags); !res.has_value())
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

auto VulkanSwapchain::setupExtentAndViewport(VkPhysicalDevice tPhysDevice) -> void
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(tPhysDevice, mSurface, &mSurfaceCap);

    FramebufferSize const framebuffer = mWindowSurface->GetFramebufferSize();

    mExtent.width = mExtent3D.width =
        std::clamp(framebuffer.width, mSurfaceCap.minImageExtent.width, mSurfaceCap.maxImageExtent.width);
    mExtent.height = mExtent3D.height =
        std::clamp(framebuffer.height, mSurfaceCap.minImageExtent.height, mSurfaceCap.maxImageExtent.height);
    mExtent3D.depth = 1U;

    mScissor.offset.x = 0;
    mScissor.offset.y = 0;
    mScissor.extent   = mExtent;

    mViewport.x        = 0.0F;
    mViewport.y        = 0.0F;
    mViewport.width    = static_cast<float>(mExtent.width);
    mViewport.height   = static_cast<float>(mExtent.height);
    mViewport.minDepth = 0.0F;
    mViewport.maxDepth = 1.0F;

    mImgCount = mSurfaceCap.minImageCount + 1U;
    if (mSurfaceCap.maxImageCount > 0U && mImgCount > mSurfaceCap.maxImageCount)
    {
        mImgCount = mSurfaceCap.maxImageCount;
    }
}

auto VulkanSwapchain::selectFormatAndMode(VulkanDevice const*                    tDevice,
                                          std::vector<VkSurfaceFormatKHR> const& tFormats,
                                          std::vector<VkPresentModeKHR> const&   tModes) -> bool
{
    bool is_surface_adequate{false};
    for (auto const& format : tFormats)
    {
        if (tDevice->CheckSurfaceFormatSupport(format))
        {
            mSurfaceFormat      = format;
            is_surface_adequate = true;
        }
    }
    for (auto const mode : tModes)
    {
        if (tDevice->CheckPresentModeSupport(mode))
        {
            mPresentMode        = mode;
            is_surface_adequate = true;
        }
    }
    return is_surface_adequate;
}

auto VulkanSwapchain::createSwapchainHandle(VkSurfaceTransformFlagsKHR tTransformFlags) -> Result<void>
{
    VkSwapchainCreateInfoKHR info{
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .surface               = mSurface,
        .minImageCount         = mImgCount,
        .imageFormat           = mSurfaceFormat.format,
        .imageColorSpace       = mSurfaceFormat.colorSpace,
        .imageExtent           = mExtent,
        .imageArrayLayers      = 1U,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0U,
        .pQueueFamilyIndices   = nullptr,
        .preTransform          = static_cast<VkSurfaceTransformFlagBitsKHR>(tTransformFlags),
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = mPresentMode,
        .clipped               = VK_TRUE,
        .oldSwapchain          = VK_NULL_HANDLE,
    };

    // Enable transfer source on swap chain images if supported
    if (0U != (mSurfaceCap.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT))
    {
        info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    // Enable transfer destination on swap chain images if supported
    if (0U != (mSurfaceCap.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
    {
        info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    CHECK_VK_SUCCESS_OR_ERR(vkCreateSwapchainKHR(mDevice, &info, nullptr, &mSwapchain),
                            RenderingErrc::kFailedCreateSwapchain);

    return {};
}

auto VulkanSwapchain::createImageViews() -> Result<void>
{
    vkGetSwapchainImagesKHR(mDevice, mSwapchain, &mImgCount, nullptr);
    mImages.resize(static_cast<std::size_t>(mImgCount));
    vkGetSwapchainImagesKHR(mDevice, mSwapchain, &mImgCount, mImages.data());
    mImageViews.resize(static_cast<std::size_t>(mImgCount));

    LogInfo("Received {} swapchain images.", mImgCount);

    auto const img_count = static_cast<std::size_t>(mImgCount);

    for (std::size_t i = 0U; i < img_count; ++i)
    {
        VkImageViewCreateInfo const create_info{
            .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext    = nullptr,
            .flags    = 0U,
            .image    = mImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format   = mSurfaceFormat.format,
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

        CHECK_VK_SUCCESS_OR_ERR(vkCreateImageView(mDevice, &create_info, nullptr, &mImageViews[i]),
                                RenderingErrc::kFailedCreateSwapchainImageViews);
    }

    LogInfo("Created {} image views.", mImageViews.size());

    return {};
}

auto VulkanSwapchain::Destroy() -> Result<void>
{
    LogInfo("Destroying VulkanSwapchain and associated images...");

    std::ranges::for_each(mImageViews, [this](VkImageView& tView) {
        vkDestroyImageView(mDevice, tView, nullptr);
    });

    vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);

    mImageViews.clear();
    mImages.clear();

    return {};
}

auto VulkanSwapchain::GetSurfaceFormat() const -> VkSurfaceFormatKHR const&
{ return mSurfaceFormat; }

auto VulkanSwapchain::GetExtent() const -> VkExtent2D const&
{ return mExtent; }

[[nodiscard]] auto VulkanSwapchain::GetExtent3D() const -> VkExtent3D const&
{ return mExtent3D; }

auto VulkanSwapchain::GetScissor() const -> VkRect2D const&
{ return mScissor; }

auto VulkanSwapchain::GetViewport() const -> VkViewport const&
{ return mViewport; }

auto VulkanSwapchain::AcquireNextImage(AcquireNextImageDetails const& tDetails) -> Result<std::uint32_t>
{
    VkResult const res = vkAcquireNextImageKHR(mDevice,
                                               mSwapchain,
                                               tDetails.timeout,
                                               tDetails.semaphore,
                                               VK_NULL_HANDLE,
                                               &mCurrentImage);

    if (VK_ERROR_OUT_OF_DATE_KHR == res)
    {
        LogError("Swapchain image is out of date.");
    }

    CHECK_VK_SUCCESS_OR_ERR(res, RenderingErrc::kFailedAcquireNextImage);

    return mCurrentImage;
}

auto VulkanSwapchain::QueuePresent(VkSemaphore tWaitSemaphore) const -> Result<void>
{
    VkPresentInfoKHR const present_info{
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext              = nullptr,
        .waitSemaphoreCount = 1U,
        .pWaitSemaphores    = &tWaitSemaphore,
        .swapchainCount     = 1U,
        .pSwapchains        = &mSwapchain,
        .pImageIndices      = &mCurrentImage,
        .pResults           = nullptr,
    };

    VkResult const res = vkQueuePresentKHR(mGfxQueue, &present_info);

    if (VK_ERROR_OUT_OF_DATE_KHR == res)
    {
        LogError("Swapchain image is out of date.");
    }

    CHECK_VK_SUCCESS_OR_ERR(res, RenderingErrc::kFailedPresentQueue);

    return {};
}

auto VulkanSwapchain::GetCurrentImage() const -> VkImage
{ return GetImage(mCurrentImage); }

auto VulkanSwapchain::GetCurrentImageIndex() const -> std::uint32_t
{ return mCurrentImage; }

auto VulkanSwapchain::GetCurrentImageView() const -> VkImageView
{ return GetImageView(mCurrentImage); }

auto VulkanSwapchain::GetImage(std::uint32_t tIndex) const -> VkImage
{ return mImages[static_cast<std::size_t>(tIndex)]; }

auto VulkanSwapchain::GetImageView(std::uint32_t tIndex) const -> VkImageView
{ return mImageViews[static_cast<std::size_t>(tIndex)]; }

auto VulkanSwapchain::GetImageCount() const -> std::uint32_t
{ return mImgCount; }

}// namespace polos::rendering
