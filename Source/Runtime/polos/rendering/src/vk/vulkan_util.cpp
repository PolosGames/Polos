///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "vk/vulkan_util.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "resources/buffer_description.hpp"
#include "resources/gpu_buffer.hpp"
#include "vk/common.hpp"
#include "vk/render_context.hpp"

#include <stb_image.h>

#include <cassert>
#include <cstdint>

namespace polos::rendering::util
{

auto CreateImageView(VkDevice tDevice, VkImage tImage, VkFormat tFormat, VkImageAspectFlags tAspectFlags) -> VkImageView
{
    VkImageViewCreateInfo const img_view_info{
        .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0U,
        .image    = tImage,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format   = tFormat,
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
        .subresourceRange = {
            .aspectMask     = tAspectFlags,
            .baseMipLevel   = 0U,
            .levelCount     = 1U,
            .baseArrayLayer = 0U,
            .layerCount     = 1U,
        },
    };

    VkImageView img_view{VK_NULL_HANDLE};
    assert(VK_SUCCESS == vkCreateImageView(tDevice, &img_view_info, nullptr, &img_view));

    return img_view;
}

namespace
{

auto AccessMaskForLayout(VkImageLayout tLayout) -> VkAccessFlags
{
    switch (tLayout)
    {
        case VK_IMAGE_LAYOUT_UNDEFINED:
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: return 0U;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: return VK_ACCESS_TRANSFER_WRITE_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: return VK_ACCESS_TRANSFER_READ_BIT;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: return VK_ACCESS_SHADER_READ_BIT;
        default: return 0U;
    }
}

}// namespace

void TransitionImageLayout(
    VkCommandBuffer      tCommandBuffer,
    VkImage              tImage,
    VkImageLayout        tOldLayout,
    VkImageLayout        tNewLayout,
    VkPipelineStageFlags tSrcStageMask,
    VkPipelineStageFlags tDstStageMask)
{
    VkImageMemoryBarrier img_mem_barrier{
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = AccessMaskForLayout(tOldLayout),
        .dstAccessMask       = AccessMaskForLayout(tNewLayout),
        .oldLayout           = tOldLayout,
        .newLayout           = tNewLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = tImage,
        .subresourceRange    = {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0U,
            .levelCount     = 1U,
            .baseArrayLayer = 0U,
            .layerCount     = 1U,
        },
    };

    if (tNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        img_mem_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    vkCmdPipelineBarrier(
        tCommandBuffer,
        tSrcStageMask,
        tDstStageMask,
        0U,
        0U,
        nullptr,
        0U,
        nullptr,
        1U,
        &img_mem_barrier);
}

void CopyBufferToImage(VkCommandBuffer tCommandBuffer, VkBuffer tBuffer, VkImage tImage, VkExtent3D tExtent)
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
        .imageOffset = {0, 0, 0},
        .imageExtent = tExtent,
    };

    vkCmdCopyBufferToImage(tCommandBuffer, tBuffer, tImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1U, &copy_region);
}

void CopyImageToImage(
    VkCommandBuffer tCommandBuffer,
    VkImage         tSrcImage,
    VkImage         tDstImage,
    VkExtent3D      tSrcExtent,
    VkExtent3D      tDstExtent)
{
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
                    .x = static_cast<std::int32_t>(tSrcExtent.width),
                    .y = static_cast<std::int32_t>(tSrcExtent.height),
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
                .x = static_cast<std::int32_t>(tDstExtent.width),
                .y = static_cast<std::int32_t>(tDstExtent.height),
                .z = 1,
            },
        },
    };

    VkBlitImageInfo2 blit_info{
        .sType          = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
        .pNext          = nullptr,
        .srcImage       = tSrcImage,
        .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .dstImage       = tDstImage,
        .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .regionCount    = 1,
        .pRegions       = &blit_region,
        .filter         = VK_FILTER_LINEAR,
    };

    vkCmdBlitImage2(tCommandBuffer, &blit_info);
}

auto CreateFramebuffer(
    VkDevice               tDevice,
    VkRenderPass           tRPass,
    std::span<VkImageView> tAttachments,
    VkExtent2D const&      tExtent) -> VkFramebuffer
{
    VkFramebufferCreateInfo const fb_info{
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0U,
        .renderPass      = tRPass,
        .attachmentCount = VK_SIZE_CAST(tAttachments.size()),
        .pAttachments    = tAttachments.data(),
        .width           = tExtent.width,
        .height          = tExtent.height,
        .layers          = 1U,
    };

    VkFramebuffer framebuffer{VK_NULL_HANDLE};
    assert(vkCreateFramebuffer(tDevice, &fb_info, nullptr, &framebuffer) == VK_SUCCESS);

    return framebuffer;
}

auto LoadImageResourceToGpuImage(
    const char*          tResourcePath,
    VkFormat             tFormat,
    VkImageUsageFlags    tUsage,
    VkImageLayout        tFinalLayout,
    VkPipelineStageFlags tFinalPipelineStage) -> Result<std::unique_ptr<GpuImage>>
{
    std::int32_t width{0};
    std::int32_t height{0};

    std::uint8_t* pixels = stbi_load(tResourcePath, &width, &height, nullptr, STBI_rgb_alpha);
    if (nullptr == pixels)
    {
        return ErrorType{RenderingErrc::kFailedLoadImage};
    }

    VkDeviceSize const image_size = static_cast<VkDeviceSize>(width) * static_cast<VkDeviceSize>(height) * 4U;

    auto img_buf = GpuBuffer::Create(
        BufferDescription{
            .size       = image_size,
            .usage      = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .residence  = MemoryResidence::kDevice,
            .hostAccess = HostAccessFlags::kSeqWrite,
        });

    std::unique_ptr<GpuBuffer> textureBuf = std::move(*img_buf);
    textureBuf->Write(pixels, image_size);

    stbi_image_free(pixels);

    auto image_create_res = GpuImage::Create(
        ImageDescription{
            .extent = {.width = VK_SIZE_CAST(width), .height = VK_SIZE_CAST(height), .depth = 1U},
            .format = tFormat,
            .usage  = tUsage | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        });

    std::unique_ptr<GpuImage> textureImg = std::move(*image_create_res);

    // Transition image layout to be optimal for receiving data transfer from staging buffer
    {
        VkCommandBuffer cmdBuf = RenderContext::BeginSingleTimeCommands();

        util::TransitionImageLayout(
            cmdBuf,
            textureImg->img,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT);

        RenderContext::EndSingleTimeCommands(cmdBuf);
    }

    // Copy data from staging buffer to texture image
    {
        VkCommandBuffer cmdBuf = RenderContext::BeginSingleTimeCommands();

        util::CopyBufferToImage(
            cmdBuf,
            textureBuf->buffer,
            textureImg->img,
            VkExtent3D{
                .width  = VK_SIZE_CAST(width),
                .height = VK_SIZE_CAST(height),
                .depth  = 1U,
            });

        RenderContext::EndSingleTimeCommands(cmdBuf);
    }

    // Transition image layout to be optimal for Shader read access
    {
        VkCommandBuffer cmdBuf = RenderContext::BeginSingleTimeCommands();

        util::TransitionImageLayout(
            cmdBuf,
            textureImg->img,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            tFinalLayout,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            tFinalPipelineStage);

        RenderContext::EndSingleTimeCommands(cmdBuf);
    }

    return textureImg;
}

}// namespace polos::rendering::util
