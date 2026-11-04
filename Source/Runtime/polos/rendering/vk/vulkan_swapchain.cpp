//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vk/vulkan_swapchain.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/resources/image.hpp"
#include "polos/rendering/vk/common.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/vulkan_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/utils/error_code.hpp"

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cstdint>

namespace polos::rendering
{

VulkanSwapchain::VulkanSwapchain() = default;

VulkanSwapchain::~VulkanSwapchain() = default;

auto VulkanSwapchain::Create(CreationDetails const& tDetails) -> Result<void>
{
    mSurface       = RenderContext::GetVulkanContext().surface;
    mGfxQueue      = RenderContext::GetVulkanDevice().gfxQueue;
    mDevice        = RenderContext::GetVulkanDevice().logiDevice;
    mWindowSurface = tDetails.windowSurface;

    setupExtentAndViewport(RenderContext::GetVulkanDevice().physDevice);

    if (!selectFormatAndMode(tDetails.preferredSurfaceFormats, tDetails.preferredPresentModes))
    {
        return ErrorType{RenderingErrc::kNoAdequateSurface};
    }

    if (!createSwapchainHandle(tDetails.transformFlags))
    {
        LogError("Failed to create the swapchain.");
        return ErrorType{RenderingErrc::kGenericError};
    }

    LogInfo("Swapchain object created, receiving images for it...");

    if (auto res = createImageViews(); !res.has_value())
    {
        Destroy();
        return ErrorType{res.error()};
    }

    LogInfo("Swapchain creation done!");

    return {};
}

auto VulkanSwapchain::setupExtentAndViewport(VkPhysicalDevice tPhysDevice) -> void
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(tPhysDevice, mSurface, &mSurfaceCap);

    platform::FramebufferSize const framebuffer = mWindowSurface->GetFramebufferSize();

    mExtent.width = mExtent3D.width =
        std::clamp(framebuffer.width, mSurfaceCap.minImageExtent.width, mSurfaceCap.maxImageExtent.width);
    mExtent.height = mExtent3D.height =
        std::clamp(framebuffer.height, mSurfaceCap.minImageExtent.height, mSurfaceCap.maxImageExtent.height);
    mExtent3D.depth = 1U;

    mRenderArea.offset.x = 0;
    mRenderArea.offset.y = 0;
    mRenderArea.extent   = mExtent;

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

auto VulkanSwapchain::selectFormatAndMode(
    std::vector<VkSurfaceFormatKHR> const& tFormats,
    std::vector<VkPresentModeKHR> const&   tModes) -> bool
{
    bool is_surface_adequate{false};
    for (auto const& format : tFormats)
    {
        if (RenderContext::GetVulkanDevice().CheckSurfaceFormatSupport(format))
        {
            mSurfaceFormat      = format;
            is_surface_adequate = true;
        }
    }
    for (auto const mode : tModes)
    {
        if (RenderContext::GetVulkanDevice().CheckPresentModeSupport(mode))
        {
            mPresentMode        = mode;
            is_surface_adequate = true;
        }
    }
    return is_surface_adequate;
}

auto VulkanSwapchain::createSwapchainHandle(VkSurfaceTransformFlagsKHR tTransformFlags) -> bool
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

    return VK_SUCCESS == vkCreateSwapchainKHR(mDevice, &info, nullptr, &mSwapchain);
}

auto VulkanSwapchain::createImageViews() -> Result<void>
{
    vkGetSwapchainImagesKHR(mDevice, mSwapchain, &mImgCount, nullptr);
    mImages.resize(static_cast<std::size_t>(mImgCount));
    vkGetSwapchainImagesKHR(mDevice, mSwapchain, &mImgCount, mImages.data());
    mImageViews.resize(static_cast<std::size_t>(mImgCount));
    mImageResources.resize(static_cast<std::size_t>(mImgCount));

    LogInfo("Received {} swapchain images.", mImgCount);

    auto const img_count = static_cast<std::size_t>(mImgCount);

    for (std::size_t i = 0U; i < img_count; ++i)
    {
        auto resource =
            Image::Create(ImageDescription{.extent = mExtent3D, .format = mSurfaceFormat.format}, mImages[i]);
        if (!resource)
        {
            return ErrorType{RenderingErrc::kGenericError};
        }
        mImageResources[i] = std::move(resource);
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

        CHECK_VK_SUCCESS_OR_ERR(
            vkCreateImageView(mDevice, &create_info, nullptr, &mImageViews[i]),
            RenderingErrc::kFailedCreateSwapchainImageViews);
    }

    LogInfo("Created {} image views.", mImageViews.size());

    return {};
}

auto VulkanSwapchain::Destroy() -> void
{
    LogInfo("Destroying VulkanSwapchain and associated images...");

    std::ranges::for_each(mImageViews, [this](VkImageView& tView) {
        vkDestroyImageView(mDevice, tView, nullptr);
    });

    mImageResources.clear();
    if (mSwapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);
        mSwapchain = VK_NULL_HANDLE;
    }

    mImageViews.clear();
    mImages.clear();
    mImgCount      = 0U;
    mCurrentImage  = 0U;
    mDevice        = VK_NULL_HANDLE;
    mGfxQueue      = VK_NULL_HANDLE;
    mSurface       = VK_NULL_HANDLE;
    mWindowSurface = nullptr;
}

auto VulkanSwapchain::GetSurfaceFormat() const -> VkSurfaceFormatKHR const&
{
    return mSurfaceFormat;
}

auto VulkanSwapchain::GetExtent() const -> VkExtent2D const&
{
    return mExtent;
}

[[nodiscard]] auto VulkanSwapchain::GetExtent3D() const -> VkExtent3D const&
{
    return mExtent3D;
}

auto VulkanSwapchain::GetScissor() const -> VkRect2D const&
{
    return mScissor;
}

auto VulkanSwapchain::GetViewport() const -> VkViewport const&
{
    return mViewport;
}

auto VulkanSwapchain::GetRenderArea() const -> VkRect2D const&
{
    return mRenderArea;
}

auto VulkanSwapchain::AcquireNextImage(AcquireNextImageDetails const& tDetails) -> std::uint32_t
{
    std::uint32_t  imageIndex{kInvalidImageIndex};
    VkResult const res =
        vkAcquireNextImageKHR(mDevice, mSwapchain, tDetails.timeout, tDetails.semaphore, VK_NULL_HANDLE, &imageIndex);

    if (VK_ERROR_OUT_OF_DATE_KHR == res)
    {
        LogError("Swapchain image is out of date.");
        return kInvalidImageIndex;
    }

    if (VK_SUCCESS != res)
    {
        LogWarn("Failed to acquire next swapchain image (VkResult: {}).", static_cast<std::int32_t>(res));
        return kInvalidImageIndex;
    }

    mCurrentImage = imageIndex;

    // The submit waits for acquisition at TRANSFER. Put the first image transition
    // in that scope as well, retaining the layout from the previous presentation.
    auto& image = *mImageResources[mCurrentImage];
    image.SetUse({
        .layout   = image.GetUse().layout,
        .stages   = VK_PIPELINE_STAGE_TRANSFER_BIT,
        .accesses = 0U,
    });

    return mCurrentImage;
}

auto VulkanSwapchain::QueuePresent(VkSemaphore tWaitSemaphore) const -> bool
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

    return VK_SUCCESS == vkQueuePresentKHR(mGfxQueue, &present_info);
}

auto VulkanSwapchain::GetCurrentImageResource() const -> std::shared_ptr<Image>
{
    return mImageResources[mCurrentImage];
}

auto VulkanSwapchain::GetCurrentImage() const -> VkImage
{
    return GetImage(mCurrentImage);
}

auto VulkanSwapchain::GetCurrentImageIndex() const -> std::uint32_t
{
    return mCurrentImage;
}

auto VulkanSwapchain::GetCurrentImageView() const -> VkImageView
{
    return GetImageView(mCurrentImage);
}

auto VulkanSwapchain::GetImage(std::uint32_t tIndex) const -> VkImage
{
    return mImages[static_cast<std::size_t>(tIndex)];
}

auto VulkanSwapchain::GetImageView(std::uint32_t tIndex) const -> VkImageView
{
    return mImageViews[static_cast<std::size_t>(tIndex)];
}

auto VulkanSwapchain::GetImageCount() const -> std::uint32_t
{
    return mImgCount;
}

}// namespace polos::rendering
