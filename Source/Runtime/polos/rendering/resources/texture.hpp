///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RESOURCES_TEXTURE_HPP
#define POLOS_RENDERING_RESOURCES_TEXTURE_HPP

#include "polos/rendering/resources/image.hpp"
#include "polos/utils/string_id.hpp"

#include <vulkan/vulkan.h>

#include <memory>

namespace polos::rendering
{

// using TextureIndex   = std::uint16_t;
// using TextureVersion = std::uint16_t;

// struct TextureHandle
// {
//     TextureIndex   index{0U};
//     TextureVersion version{0U};
// };

// inline constexpr TextureIndex   kInvalidTextureIndex{std::numeric_limits<std::uint16_t>::max()};
// inline constexpr TextureVersion kInvalidTextureVersion{std::numeric_limits<std::uint16_t>::max()};
// inline constexpr TextureHandle  kInvalidTextureHandle{
//     .index   = kInvalidTextureIndex,
//     .version = kInvalidTextureVersion,
// };

struct TextureDetails
{
    VkFilter             minFilter{VK_FILTER_LINEAR};
    VkFilter             magFilter{VK_FILTER_LINEAR};
    VkSamplerMipmapMode  mipmap{VK_SAMPLER_MIPMAP_MODE_LINEAR};
    VkSamplerAddressMode addressModeU{VK_SAMPLER_ADDRESS_MODE_REPEAT};
    VkSamplerAddressMode addressModeV{VK_SAMPLER_ADDRESS_MODE_REPEAT};
};

struct Texture
{
    utils::string_id       name;// Also the ResourceId
    std::unique_ptr<Image> image;
    VkImageView            view{VK_NULL_HANDLE};
    VkSampler              sampler{VK_NULL_HANDLE};

    TextureDetails details;

    [[nodiscard]] static auto
    Create(utils::string_id tName, std::unique_ptr<rendering::Image> tImg, TextureDetails const& tDetails)
        -> std::unique_ptr<Texture>;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RESOURCES_TEXTURE_HPP
