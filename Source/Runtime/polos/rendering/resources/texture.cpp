///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/resources/texture.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"

namespace polos::rendering
{

auto Texture::Create(
    utils::string_id const            tName,
    std::unique_ptr<rendering::Image> tImg,
    TextureDetails const&             tDetails) -> std::unique_ptr<Texture>
{
    // VkPhysicalDeviceProperties phys_dev_props{};
    // vkGetPhysicalDeviceProperties(RenderContext::GetVulkanDevice().physDevice, &phys_dev_props);

    VkImageViewCreateInfo const viewInfo{
        .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = 0U,
        .image            = tImg->img,
        .viewType         = VK_IMAGE_VIEW_TYPE_2D,
        .format           = tImg->format,
        .components       = {},
        .subresourceRange = tImg->subresourceRange,
    };

    VkImageView view{VK_NULL_HANDLE};
    if (VK_SUCCESS != vkCreateImageView(RenderContext::GetVulkanDevice().logiDevice, &viewInfo, nullptr, &view))
    {
        LogError("Could not create a image view for the texture.");
        return nullptr;
    }

    VkSamplerCreateInfo const sampler_info{
        .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .magFilter               = tDetails.magFilter,
        .minFilter               = tDetails.minFilter,
        .mipmapMode              = tDetails.mipmap,
        .addressModeU            = tDetails.addressModeU,
        .addressModeV            = tDetails.addressModeV,
        .addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias              = 0.0F,
        .anisotropyEnable        = VK_FALSE,
        .maxAnisotropy           = 1.0F,
        .compareEnable           = VK_FALSE,
        .compareOp               = VK_COMPARE_OP_ALWAYS,
        .minLod                  = 0.0F,
        .maxLod                  = static_cast<float>(tImg->subresourceRange.levelCount - 1U),
        .borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };

    VkSampler sampler{VK_NULL_HANDLE};
    if (VK_SUCCESS != vkCreateSampler(RenderContext::GetVulkanDevice().logiDevice, &sampler_info, nullptr, &sampler))
    {
        LogError("Could not create a sampler for the texture");
        vkDestroyImageView(RenderContext::GetVulkanDevice().logiDevice, view, nullptr);
        return nullptr;
    }

    return std::make_unique<Texture>(tName, std::move(tImg), view, sampler, tDetails);
}

}// namespace polos::rendering
