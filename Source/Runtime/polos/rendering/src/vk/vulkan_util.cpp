///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "vk/vulkan_util.hpp"

#include "vk/common.hpp"

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

void TransitionImageLayout(VkCommandBuffer      tCommandBuffer,
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

    vkCmdPipelineBarrier(tCommandBuffer,
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

void CopyImageToImage(VkCommandBuffer tCommandBuffer,
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

auto CreateFramebuffer(VkDevice               tDevice,
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

}// namespace polos::rendering::util
