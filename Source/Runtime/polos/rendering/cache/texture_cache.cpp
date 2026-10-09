///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/cache/texture_cache.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/resources/image.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/resource/image.hpp"
#include "polos/resource/resource_manager.hpp"

#include <array>
#include <limits>

namespace polos::rendering
{

TextureCache* TextureCache::sInstance{nullptr};

TextureCache::TextureCache()
{
    sInstance = this;
}

TextureCache::~TextureCache()
{
    sInstance = nullptr;
}

auto TextureCache::Create(CreationDetails const& /*tDetails*/) -> Result<void>
{
    // White texture
    auto image = Image::Create(
        ImageDescription{
            .extent      = VkExtent3D{.width = 1U, .height = 1U, .depth = 1U},
            .format      = VK_FORMAT_R8G8B8A8_SRGB,
            .usage       = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .mipLevels   = 1U,
            .arrayLayers = 1U,
            .samples     = VK_SAMPLE_COUNT_1_BIT,
        },
        VK_FORMAT_R8G8B8A8_SRGB,
        std::array<std::uint8_t, 4U>({0xFF, 0xFF, 0xFF, 0xFF}));

    if (nullptr == image)
    {
        return ErrorType{RenderingErrc::kFailedCreateDefaultTexture};
    }

    auto texture = Texture::Create("texture.polos_default_white"_sid, std::move(image), {});
    if (nullptr == texture)
    {
        return ErrorType{RenderingErrc::kFailedCreateDefaultTexture};
    }
    mTextures.push_back(std::move(texture));

    return {};
}

auto TextureCache::CreateTexture(resource::ResourceId const tResource, TextureDetails const& tDetails) -> std::size_t
{
    auto image = resource::ResourceManager::Instance().GetImage(tResource);
    if (nullptr == image)
    {
        LogError("Image not found in registry");
        return std::numeric_limits<std::size_t>::max();
    }
    auto const width  = static_cast<std::uint32_t>(image->width);
    auto const height = static_cast<std::uint32_t>(image->height);
    // Resource images are decoded to RGBA8 by the image loader.
    constexpr VkFormat format{VK_FORMAT_R8G8B8A8_SRGB};

    auto gpuImage = Image::Create(
        ImageDescription{
            .extent      = VkExtent3D{.width = width, .height = height, .depth = 1U},
            .format      = format,
            .usage       = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .mipLevels   = 1U,
            .arrayLayers = 1U,
            .samples     = VK_SAMPLE_COUNT_1_BIT,
        },
        format,
        image->data);

    if (nullptr == gpuImage)
    {
        LogError("Image could not be created from the resource");
        return std::numeric_limits<std::size_t>::max();
    }

    auto& textures = sInstance->mTextures;

    auto const index   = textures.size();
    auto       texture = Texture::Create(tResource, std::move(gpuImage), tDetails);
    if (nullptr == texture)
    {
        LogError("Texture could not be created from the image");
        return std::numeric_limits<std::size_t>::max();
    }

    textures.push_back(std::move(texture));

    return index;
}

auto TextureCache::GetTexture(resource::ResourceId const tResource) -> Texture const*
{
    auto const itr = std::ranges::find_if(sInstance->mTextures, [&tResource](std::unique_ptr<Texture>& tTexture) {
        return tTexture->name == tResource;
    });

    if (itr == sInstance->mTextures.end())
    {
        return nullptr;
    }

    return itr->get();
}

auto TextureCache::GetTextureAtIndex(std::size_t tIndex) -> Texture const*
{
    if (tIndex >= sInstance->mTextures.size())
    {
        return nullptr;
    }

    return sInstance->mTextures[tIndex].get();
}

auto TextureCache::Destroy() -> void
{
    for (std::size_t i{0U}; i < mTextures.size(); ++i)
    {
        vkDestroyImageView(RenderContext::GetVulkanDevice().logiDevice, mTextures[i]->view, nullptr);
        vkDestroySampler(RenderContext::GetVulkanDevice().logiDevice, mTextures[i]->sampler, nullptr);
    }
    mTextures.clear();
}

}// namespace polos::rendering
