///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/cache/texture_cache.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/resources/image.hpp"
#include "polos/rendering/vk/descriptor_pool_builder.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"

#include <utility>

namespace polos::rendering
{

namespace
{
inline constexpr uint32_t      kMaxBindlessTextures{4096U};
inline constexpr std::uint32_t kBindlessSetIndex{0U};
}// namespace

TextureCache* TextureCache::sInstance{nullptr};

auto TextureCache::Get() -> TextureCache&
{
    return *sInstance;
}

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
    {
        VkDescriptorBindingFlags const bindingFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

        VkDescriptorSetLayoutBindingFlagsCreateInfo const bindingFlagsInfo{
            .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .pNext         = nullptr,
            .bindingCount  = 1U,
            .pBindingFlags = &bindingFlags,
        };

        VkDescriptorSetLayoutBinding const bindlessBinding{
            .binding            = 0U,
            .descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount    = kMaxBindlessTextures,
            .stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        };

        DescriptorPoolBuilder builder;
        builder.SetFlags(VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT);
        builder.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, kMaxBindlessTextures);
        builder.AddSet(1U)
            .AddBinding(bindlessBinding)
            .SetLayoutFlags(VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT)
            .SetLayoutNext(&bindingFlagsInfo);

        mBindlessPool = builder.Finalize();
    }

    if (!AddTexture(kDefaultTexture, {.width = 1U, .height = 1U}, kDefaultTextureRgba, SamplerDetails{}))
    {
        return ErrorType{RenderingErrc::kFailedCreateDefaultTexture};
    }

    return {};
}

auto TextureCache::AddTexture(ResourceId const tId, VkExtent2D const tExtent, std::span<std::uint8_t const> tPixels, SamplerDetails const& tDetails) -> bool
{
    if (auto const* texture = GetTexture(tId))
    {
        return texture->details == tDetails;
    }

    if (mTextures.size() >= kMaxBindlessTextures)
    {
        LogError("Bindless texture cache capacity ({}) exceeded", kMaxBindlessTextures);
        return false;
    }

    constexpr VkFormat format{VK_FORMAT_R8G8B8A8_SRGB};
    auto               image = Image::Create(
        ImageDescription{
            .extent      = VkExtent3D{.width = tExtent.width, .height = tExtent.height, .depth = 1U},
            .format      = format,
            .usage       = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .mipLevels   = 1U,
            .arrayLayers = 1U,
            .samples     = VK_SAMPLE_COUNT_1_BIT,
        },
        format, tPixels);
    if (image == nullptr)
    {
        LogError("Could not create a GPU image for texture '{}'.", tId);
        return false;
    }

    auto texture = Texture::Create(tId, std::move(image), tDetails);
    if (texture == nullptr)
    {
        return false;
    }

    texture->index   = static_cast<TextureIndex>(mTextures.size());
    auto const entry = mTextures.emplace(tId, std::move(texture)).first;
    writeBindlessDescriptor(entry->second->index, *entry->second);
    return true;
}

auto TextureCache::GetTexture(ResourceId const tId) const -> Texture const*
{
    auto const found = mTextures.find(tId);
    return found != mTextures.end() ? found->second.get() : nullptr;
}

auto TextureCache::GetTextureIndex(ResourceId const tId) const -> TextureIndex
{
    auto const* texture = GetTexture(tId);
    return texture != nullptr ? texture->index : kDefaultTextureIndex;
}

auto TextureCache::GetBindlessSetLayout() -> VkDescriptorSetLayout
{
    return mBindlessPool.GetSetLayoutAtIndex(kBindlessSetIndex);
}

auto TextureCache::GetBindlessDescriptorSet() -> VkDescriptorSet
{
    return mBindlessPool.GetSetHandle(kBindlessSetIndex);
}

auto TextureCache::writeBindlessDescriptor(TextureIndex const tIndex, Texture const& tTexture) -> void
{
    VkDescriptorImageInfo const imageInfo{
        .sampler     = tTexture.sampler,
        .imageView   = tTexture.view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    mBindlessPool.UpdateImage(kBindlessSetIndex, 0U, tIndex, imageInfo);
}

auto TextureCache::Destroy() -> void
{
    mBindlessPool = DescriptorPool{};

    VkDevice device = VulkanDevice::Get().logiDevice;
    for (auto const& [_, texture] : mTextures)
    {
        vkDestroyImageView(device, texture->view, nullptr);
        vkDestroySampler(device, texture->sampler, nullptr);
    }
    mTextures.clear();
}

}// namespace polos::rendering
