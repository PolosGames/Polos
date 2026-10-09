///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "resources/gpu_image.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "resources/gpu_buffer.hpp"
#include "vk/common.hpp"

#include <utility>

namespace
{

auto AspectMaskForFormat(VkFormat tFormat) -> VkImageAspectFlags
{
    switch (tFormat)
    {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT: return VK_IMAGE_ASPECT_DEPTH_BIT;
        case VK_FORMAT_S8_UINT: return VK_IMAGE_ASPECT_STENCIL_BIT;
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT: return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        default: return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

}// namespace

namespace polos::rendering
{

auto ImageUse::ForLayout(VkImageLayout tLayout) -> ImageUse
{
    switch (tLayout)
    {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            return {
                .layout   = tLayout,
                .stages   = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                .accesses = 0U,
            };
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            return {
                .layout   = tLayout,
                .stages   = VK_PIPELINE_STAGE_HOST_BIT,
                .accesses = VK_ACCESS_HOST_WRITE_BIT,
            };
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            return {
                .layout   = tLayout,
                .stages   = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                .accesses = 0U,
            };
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return {
                .layout   = tLayout,
                .stages   = VK_PIPELINE_STAGE_TRANSFER_BIT,
                .accesses = VK_ACCESS_TRANSFER_WRITE_BIT,
            };
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return {
                .layout   = tLayout,
                .stages   = VK_PIPELINE_STAGE_TRANSFER_BIT,
                .accesses = VK_ACCESS_TRANSFER_READ_BIT,
            };
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return {
                .layout   = tLayout,
                .stages   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .accesses = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            };
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return {
                .layout   = tLayout,
                .stages   = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                .accesses = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            };
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
            return {
                .layout   = tLayout,
                .stages   = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                .accesses = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT,
            };
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return {
                .layout   = tLayout,
                .stages   = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                .accesses = VK_ACCESS_SHADER_READ_BIT,
            };
        default:
            return {
                .layout   = tLayout,
                .stages   = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                .accesses = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
            };
    }
}

VmaAllocator GpuImage::sAllocator{VK_NULL_HANDLE};

GpuImage::GpuImage(GpuImage&& tOther)
{ swap(tOther); }

GpuImage::~GpuImage()
{ destroy(); }

auto GpuImage::operator=(GpuImage&& tOther) -> GpuImage&
{
    if (this == &tOther)
    {
        return *this;
    }

    destroy();
    swap(tOther);

    return *this;
}

auto GpuImage::Create(ImageDescription const& tDesc) -> Result<std::unique_ptr<GpuImage>>
{
    VkImageCreateInfo const imgInfo{
        .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .imageType             = 1U < tDesc.extent.depth ? VK_IMAGE_TYPE_3D : VK_IMAGE_TYPE_2D,
        .format                = tDesc.format,
        .extent                = tDesc.extent,
        .mipLevels             = tDesc.mipLevels,
        .arrayLayers           = tDesc.arrayLayers,
        .samples               = tDesc.samples,
        .tiling                = VK_IMAGE_TILING_OPTIMAL,
        .usage                 = tDesc.usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0U,
        .pQueueFamilyIndices   = nullptr,
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VmaAllocationCreateInfo const allocInfo{
        .flags          = 0U,
        .usage          = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        .requiredFlags  = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        .preferredFlags = 0U,
        .memoryTypeBits = 0U,
        .pool           = VK_NULL_HANDLE,
        .pUserData      = nullptr,
        .priority       = 1.0F,
    };

    auto image               = std::unique_ptr<GpuImage>(new GpuImage());
    image->extent            = tDesc.extent;
    image->format            = tDesc.format;
    image->mSubresourceRange = {
        .aspectMask     = AspectMaskForFormat(tDesc.format),
        .baseMipLevel   = 0U,
        .levelCount     = tDesc.mipLevels,
        .baseArrayLayer = 0U,
        .layerCount     = tDesc.arrayLayers,
    };

    CHECK_VK_SUCCESS_OR_ERR(
        vmaCreateImage(sAllocator, &imgInfo, &allocInfo, &image->img, &image->allocation, nullptr),
        RenderingErrc::kFailedCreateImage);
    image->mOwnsImage = true;

    return image;
}

auto GpuImage::Create(ImageDescription const& tDesc, VkImage tImg, ImageUse tInitialUse)
    -> Result<std::unique_ptr<GpuImage>>
{
    if (tImg == VK_NULL_HANDLE)
    {
        return ErrorType{RenderingErrc::kGenericError};
    }
    auto image               = std::unique_ptr<GpuImage>(new GpuImage());
    image->img               = tImg;
    image->extent            = tDesc.extent;
    image->format            = tDesc.format;
    image->mCurrentUse       = tInitialUse;
    image->mSubresourceRange = {
        .aspectMask     = AspectMaskForFormat(tDesc.format),
        .baseMipLevel   = 0U,
        .levelCount     = tDesc.mipLevels,
        .baseArrayLayer = 0U,
        .layerCount     = tDesc.arrayLayers,
    };
    return image;
}

auto GpuImage::ChangeLayout(VkCommandBuffer tCmdBuf, VkImageLayout tNewLayout) -> Result<void>
{ return ChangeLayout(tCmdBuf, ImageUse::ForLayout(tNewLayout)); }

auto GpuImage::ChangeLayout(VkCommandBuffer tCmdBuf, ImageUse tNewUse) -> Result<void>
{
    if (img == VK_NULL_HANDLE || tCmdBuf == VK_NULL_HANDLE || mCurrentUse.stages == 0U || tNewUse.stages == 0U ||
        tNewUse.layout == VK_IMAGE_LAYOUT_UNDEFINED || tNewUse.layout == VK_IMAGE_LAYOUT_PREINITIALIZED)
    {
        LogError("Cannot record an image transition with invalid handles, stages or destination layout");
        return ErrorType{RenderingErrc::kGenericError};
    }

    VkImageMemoryBarrier const barrier{
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = mCurrentUse.accesses,
        .dstAccessMask       = tNewUse.accesses,
        .oldLayout           = mCurrentUse.layout,
        .newLayout           = tNewUse.layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = img,
        .subresourceRange    = mSubresourceRange,
    };

    // Equal layouts can still require a memory dependency (e.g. two storage-image uses).
    vkCmdPipelineBarrier(tCmdBuf, mCurrentUse.stages, tNewUse.stages, 0U, 0U, nullptr, 0U, nullptr, 1U, &barrier);
    mCurrentUse = tNewUse;
    return {};
}

auto GpuImage::BlitFrom(VkCommandBuffer tCmdBuf, GpuImage& tBlitSrc) -> void
{
    if (this == &tBlitSrc)
    {
        LogWarn("Trying to blit into itself, returning...");
        return;
    }

    if (extent.width != tBlitSrc.extent.width || extent.height != tBlitSrc.extent.height ||
        extent.depth != tBlitSrc.extent.depth)
    {
        LogWarn("Trying to blit images with different sizes. Returning...");
        return;
    }

    // TODO(sorbatdev): Check VK_FORMAT_FEATURE_BLIT_SRC_BIT and VK_FORMAT_FEATURE_BLIT_DST_BIT support
    VkImageBlit2 blit_region{
        .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
        .pNext = nullptr,
        .srcSubresource =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel       = 0U,
                .baseArrayLayer = 0U,
                .layerCount     = 1U,
            },
        .srcOffsets =
            {
                {
                    .x = 0,
                    .y = 0,
                    .z = 0,
                },
                {
                    .x = static_cast<std::int32_t>(tBlitSrc.extent.width),
                    .y = static_cast<std::int32_t>(tBlitSrc.extent.height),
                    .z = 1,
                },
            },
        .dstSubresource =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel       = 0U,
                .baseArrayLayer = 0U,
                .layerCount     = 1U,
            },
        .dstOffsets = {
            {
                .x = 0,
                .y = 0,
                .z = 0,
            },
            {
                .x = static_cast<std::int32_t>(extent.width),
                .y = static_cast<std::int32_t>(extent.height),
                .z = 1,
            },
        },
    };

    VkBlitImageInfo2 blit_info{
        .sType          = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
        .pNext          = nullptr,
        .srcImage       = tBlitSrc.img,
        .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .dstImage       = img,
        .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .regionCount    = 1,
        .pRegions       = &blit_region,
        .filter         = VK_FILTER_LINEAR,
    };

    vkCmdBlitImage2(tCmdBuf, &blit_info);
}

auto GpuImage::BlitFrom(VkCommandBuffer tCmdBuf, GpuBuffer& tBlitSrc) -> void
{
    VkBufferImageCopy const copy_region{
        .bufferOffset      = 0U,
        .bufferRowLength   = 0U,
        .bufferImageHeight = 0U,
        .imageSubresource =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel       = 0U,
                .baseArrayLayer = 0U,
                .layerCount     = 1U,
            },
        .imageOffset = {.x = 0, .y = 0, .z = 0},
        .imageExtent = extent,
    };

    vkCmdCopyBufferToImage(tCmdBuf, tBlitSrc.buffer, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1U, &copy_region);
}

auto GpuImage::SetUse(ImageUse tUse) -> void
{ mCurrentUse = tUse; }

auto GpuImage::GetUse() const -> ImageUse
{ return mCurrentUse; }

auto GpuImage::destroy() -> void
{
    if (mOwnsImage && img != VK_NULL_HANDLE)
    {
        vmaDestroyImage(sAllocator, img, allocation);
    }

    allocation        = VK_NULL_HANDLE;
    img               = VK_NULL_HANDLE;
    mOwnsImage        = false;
    mCurrentUse       = {};
    mSubresourceRange = {};
}

auto GpuImage::swap(GpuImage& tOther) -> void
{
    std::swap(allocation, tOther.allocation);
    std::swap(img, tOther.img);
    std::swap(extent, tOther.extent);
    std::swap(format, tOther.format);
    std::swap(mOwnsImage, tOther.mOwnsImage);
    std::swap(mCurrentUse, tOther.mCurrentUse);
    std::swap(mSubresourceRange, tOther.mSubresourceRange);
}

}// namespace polos::rendering
