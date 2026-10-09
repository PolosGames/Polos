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
        if (VK_NULL_HANDLE == cmdBuf)
        {
            return ErrorType{RenderingErrc::kGenericError};
        }

        if (auto result = textureImg->ChangeLayout(cmdBuf, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL); !result.has_value())
        {
            RenderContext::EndSingleTimeCommands(cmdBuf);
            return ErrorType{result.error()};
        }

        RenderContext::EndSingleTimeCommands(cmdBuf);
    }

    // Copy data from staging buffer to texture image
    {
        VkCommandBuffer cmdBuf = RenderContext::BeginSingleTimeCommands();
        if (VK_NULL_HANDLE == cmdBuf)
        {
            return ErrorType{RenderingErrc::kGenericError};
        }

        textureImg->BlitFrom(cmdBuf, *textureBuf);

        RenderContext::EndSingleTimeCommands(cmdBuf);
    }

    // Transition image layout to be optimal for Shader read access
    {
        VkCommandBuffer cmdBuf = RenderContext::BeginSingleTimeCommands();
        if (VK_NULL_HANDLE == cmdBuf)
        {
            return ErrorType{RenderingErrc::kGenericError};
        }

        ImageUse finalUse = ImageUse::ForLayout(tFinalLayout);
        finalUse.stages   = tFinalPipelineStage;
        if (auto result = textureImg->ChangeLayout(cmdBuf, finalUse); !result.has_value())
        {
            RenderContext::EndSingleTimeCommands(cmdBuf);
            return ErrorType{result.error()};
        }

        RenderContext::EndSingleTimeCommands(cmdBuf);
    }

    return textureImg;
}

}// namespace polos::rendering::util
