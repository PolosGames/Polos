///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RESOURCES_TEXTURE_HPP
#define POLOS_RENDERING_RESOURCES_TEXTURE_HPP

#include "polos/rendering/common.hpp"
#include "polos/rendering/resources/image.hpp"

#include <vulkan/vulkan.h>

#include <memory>

namespace polos::rendering
{

struct SamplerDetails
{
    VkFilter             minFilter{VK_FILTER_LINEAR};
    VkFilter             magFilter{VK_FILTER_LINEAR};
    VkSamplerMipmapMode  mipmap{VK_SAMPLER_MIPMAP_MODE_LINEAR};
    VkSamplerAddressMode addressModeU{VK_SAMPLER_ADDRESS_MODE_REPEAT};
    VkSamplerAddressMode addressModeV{VK_SAMPLER_ADDRESS_MODE_REPEAT};
};

struct Texture
{
    TextureHandle  name;// Also the ResourceId
    SamplerDetails details;

    std::unique_ptr<Image> image;
    VkImageView            view{VK_NULL_HANDLE};
    VkSampler              sampler{VK_NULL_HANDLE};

    [[nodiscard]] static auto
    Create(TextureHandle tName, std::unique_ptr<rendering::Image> tImg, SamplerDetails const& tDetails)
        -> std::unique_ptr<Texture>;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RESOURCES_TEXTURE_HPP
