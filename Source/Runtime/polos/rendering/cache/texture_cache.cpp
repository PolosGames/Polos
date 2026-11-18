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
#include "polos/resource/image.hpp"
#include "polos/resource/resource_manager.hpp"

#include <algorithm>
#include <limits>
#include <tuple>
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
        VkDescriptorBindingFlags const bindingFlags =
            VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

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

    // Source-style magenta fallback.
    auto image = Image::Create(
        ImageDescription{
            .extent      = VkExtent3D{.width = 1U, .height = 1U, .depth = 1U},
            .format      = VK_FORMAT_R8G8B8A8_SRGB,
            .usage       = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .mipLevels   = 1U,
            .arrayLayers = 1U,
            .samples     = VK_SAMPLE_COUNT_1_BIT,
        },
        VK_FORMAT_R8G8B8A8_SRGB, kDefaultTextureRgba);

    if (nullptr == image)
    {
        return ErrorType{RenderingErrc::kFailedCreateDefaultTexture};
    }

    auto texture = Texture::Create(kDefaultTexture, std::move(image), {});
    if (nullptr == texture)
    {
        return ErrorType{RenderingErrc::kFailedCreateDefaultTexture};
    }
    mTextures.push_back(std::move(texture));
    writeBindlessDescriptor(kDefaultTextureIndex, *mTextures.front());

    for (auto const& material : resource::ResourceManager::Get().FetchMaterials())
    {
        if (auto source = material.lock())
        {
            auto const& baseColor = source->baseColorTexture;
            std::ignore = GetOrCreateTextureIndex(baseColor.img, baseColor.textureDetails);
        }
    }

    return {};
}

auto TextureCache::CreateTexture(resource::ResourceId const tResource, SamplerDetails const& tDetails) -> uint32_t
{
    auto& textures = mTextures;
    if (textures.size() >= kMaxBindlessTextures)
    {
        LogError("Bindless texture cache capacity ({}) exceeded", kMaxBindlessTextures);
        return std::numeric_limits<uint32_t>::max();
    }

    auto image = resource::ResourceManager::Get().GetImage(tResource);
    if (image == nullptr)
    {
        LogError("Texture image '{}' is missing from the resource catalog.", tResource);
        return std::numeric_limits<std::uint32_t>::max();
    }

    auto const         width  = static_cast<std::uint32_t>(image->width);
    auto const         height = static_cast<std::uint32_t>(image->height);
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
        format, image->data);

    if (gpuImage == nullptr)
    {
        LogError("Could not create a GPU image for texture '{}'.", tResource);
        return std::numeric_limits<std::uint32_t>::max();
    }

    auto const index   = static_cast<uint32_t>(textures.size());
    auto       texture = Texture::Create(tResource, std::move(gpuImage), tDetails);
    if (nullptr == texture)
    {
        LogError("Texture could not be created from the image");
        return std::numeric_limits<uint32_t>::max();
    }

    textures.push_back(std::move(texture));
    writeBindlessDescriptor(static_cast<TextureIndex>(index), *textures.back());

    return index;
}

auto TextureCache::GetOrCreateTextureIndex(resource::ResourceId const tResource, SamplerDetails const& tDetails)
    -> TextureIndex
{
    if (tResource == 0 || tResource == kDefaultTexture)
    {
        return kDefaultTextureIndex;
    }

    auto const index = GetTextureIndex(tResource, tDetails);
    if (index != kDefaultTextureIndex)
    {
        return index;
    }

    auto const created = CreateTexture(tResource, tDetails);
    if (created == std::numeric_limits<uint32_t>::max())
    {
        return kDefaultTextureIndex;
    }

    return static_cast<TextureIndex>(created);
}

auto TextureCache::GetTextureIndex(resource::ResourceId tResource, SamplerDetails const& tDetails) const
    -> TextureIndex
{
    for (std::size_t i{1U}; i < mTextures.size(); ++i)
    {
        if (mTextures[i]->name == tResource && mTextures[i]->details == tDetails)
        {
            return static_cast<TextureIndex>(i);
        }
    }
    return kDefaultTextureIndex;
}

auto TextureCache::GetTexture(resource::ResourceId const tResource) -> Texture const*
{
    auto const itr = std::ranges::find_if(mTextures, [&tResource](std::unique_ptr<Texture>& tTexture) {
        return tTexture->name == tResource;
    });

    if (itr == mTextures.end())
    {
        return nullptr;
    }

    return itr->get();
}

auto TextureCache::GetTextureAtIndex(std::size_t tIndex) -> Texture const*
{
    if (tIndex >= mTextures.size())
    {
        return nullptr;
    }

    return mTextures[tIndex].get();
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
    for (std::size_t i{0U}; i < mTextures.size(); ++i)
    {
        vkDestroyImageView(device, mTextures[i]->view, nullptr);
        vkDestroySampler(device, mTextures[i]->sampler, nullptr);
    }
    mTextures.clear();
}

}// namespace polos::rendering
