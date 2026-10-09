///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/vulkan_util.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/resources/buffer.hpp"
#include "polos/rendering/vk/common.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/resource/image.hpp"

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
    if (VK_SUCCESS != vkCreateImageView(tDevice, &img_view_info, nullptr, &img_view))
    {
        LogError("Could not create image view");
        return VK_NULL_HANDLE;
    }

    return img_view;
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
    if (VK_SUCCESS != vkCreateFramebuffer(tDevice, &fb_info, nullptr, &framebuffer))
    {
        return VK_NULL_HANDLE;
    }

    return framebuffer;
}


}// namespace polos::rendering::util
