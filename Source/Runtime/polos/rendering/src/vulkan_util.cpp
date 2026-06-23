///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vulkan_util.hpp"

#include <cassert>
#include <cstdint>

namespace polos::rendering::util
{

auto CreateImageView(VkDevice t_device, VkImage t_image, VkFormat t_format, VkImageAspectFlags t_aspect_flags)
    -> VkImageView
{
    VkImageViewCreateInfo const img_view_info{
        .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0U,
        .image    = t_image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format   = t_format,
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
        .subresourceRange =
            {
                .aspectMask     = t_aspect_flags,
                .baseMipLevel   = 0U,
                .levelCount     = 1U,
                .baseArrayLayer = 0U,
                .layerCount     = 1U,
            },
    };

    VkImageView img_view{VK_NULL_HANDLE};
    assert(VK_SUCCESS == vkCreateImageView(t_device, &img_view_info, nullptr, &img_view));

    return img_view;
}

namespace
{

auto AccessMaskForLayout(VkImageLayout t_layout) -> VkAccessFlags
{
    switch (t_layout)
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

void TransitionImageLayout(VkCommandBuffer      t_command_buffer,
                           VkImage              t_image,
                           VkImageLayout        t_old_layout,
                           VkImageLayout        t_new_layout,
                           VkPipelineStageFlags t_src_stage_mask,
                           VkPipelineStageFlags t_dst_stage_mask)
{
    VkImageMemoryBarrier img_mem_barrier{.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                         .pNext               = nullptr,
                                         .srcAccessMask       = AccessMaskForLayout(t_old_layout),
                                         .dstAccessMask       = AccessMaskForLayout(t_new_layout),
                                         .oldLayout           = t_old_layout,
                                         .newLayout           = t_new_layout,
                                         .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .image               = t_image,
                                         .subresourceRange    = {
                                             .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                                             .baseMipLevel   = 0U,
                                             .levelCount     = 1U,
                                             .baseArrayLayer = 0U,
                                             .layerCount     = 1U,
                                         }};

    if (t_new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        img_mem_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    vkCmdPipelineBarrier(t_command_buffer,
                         t_src_stage_mask,
                         t_dst_stage_mask,
                         0U,
                         0U,
                         nullptr,
                         0U,
                         nullptr,
                         1U,
                         &img_mem_barrier);
}

void CopyBufferToImage(VkCommandBuffer t_command_buffer, VkBuffer t_buffer, VkImage t_image, VkExtent3D t_extent)
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
        .imageExtent = t_extent,
    };

    vkCmdCopyBufferToImage(t_command_buffer, t_buffer, t_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1U, &copy_region);
}

void CopyImageToImage(VkCommandBuffer t_command_buffer,
                      VkImage         t_src_image,
                      VkImage         t_dst_image,
                      VkExtent3D      t_src_extent,
                      VkExtent3D      t_dst_extent)
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
                    .x = static_cast<std::int32_t>(t_src_extent.width),
                    .y = static_cast<std::int32_t>(t_src_extent.height),
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
        .dstOffsets =
            {
                {
                    .x = 0,
                    .y = 0,
                    .z = 0,
                },
                {
                    .x = static_cast<std::int32_t>(t_dst_extent.width),
                    .y = static_cast<std::int32_t>(t_dst_extent.height),
                    .z = 1,
                },
            },
    };

    VkBlitImageInfo2 blit_info{
        .sType          = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
        .pNext          = nullptr,
        .srcImage       = t_src_image,
        .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .dstImage       = t_dst_image,
        .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .regionCount    = 1,
        .pRegions       = &blit_region,
        .filter         = VK_FILTER_LINEAR,
    };

    vkCmdBlitImage2(t_command_buffer, &blit_info);
}

}// namespace polos::rendering::util
