///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/cache/texture_cache.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/resources/image.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/resource/image.hpp"
#include "polos/resource/resource_manager.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/string_map.hpp"

#include <algorithm>
#include <array>
#include <limits>
#include <utility>

namespace polos::rendering
{

namespace
{
inline constexpr uint32_t kMaxBindlessTextures{4096U};
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
    VkDevice device = VulkanDevice::Get().logiDevice;

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

    VkDescriptorSetLayoutCreateInfo const layoutInfo{
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = &bindingFlagsInfo,
        .flags        = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
        .bindingCount = 1U,
        .pBindings    = &bindlessBinding,
    };

    PL_ASSERT(
        VK_SUCCESS == vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &mBindlessSetLayout),
        "Failed to create bindless texture descriptor set layout");

    VkDescriptorPoolSize const poolSize{
        .type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = kMaxBindlessTextures,
    };

    VkDescriptorPoolCreateInfo const poolInfo{
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
        .maxSets       = 1U,
        .poolSizeCount = 1U,
        .pPoolSizes    = &poolSize,
    };

    PL_ASSERT(
        VK_SUCCESS == vkCreateDescriptorPool(device, &poolInfo, nullptr, &mBindlessPool),
        "Failed to create bindless texture descriptor pool");

    VkDescriptorSetAllocateInfo const allocInfo{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = mBindlessPool,
        .descriptorSetCount = 1U,
        .pSetLayouts        = &mBindlessSetLayout,
    };

    PL_ASSERT(
        VK_SUCCESS == vkAllocateDescriptorSets(device, &allocInfo, &mBindlessSet),
        "Failed to allocate bindless texture descriptor set");

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
        VK_FORMAT_R8G8B8A8_SRGB,
        kDefaultTextureRgba);

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

    return {};
}

auto TextureCache::CreateTexture(resource::ResourceId const tResource, TextureDetails const& tDetails) -> std::size_t
{
    auto& textures = mTextures;
    if (textures.size() >= kMaxBindlessTextures)
    {
        LogError("Bindless texture cache capacity ({}) exceeded", kMaxBindlessTextures);
        return std::numeric_limits<std::size_t>::max();
    }

    auto image = resource::ResourceManager::Get().GetImage(tResource);
    if (nullptr == image)
    {
        LogError("Image not found in resouce registry");
        return std::numeric_limits<std::size_t>::max();
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
        format,
        image->data);

    if (nullptr == gpuImage)
    {
        LogError("Image could not be created from the resource");
        return std::numeric_limits<std::size_t>::max();
    }

    auto const index   = textures.size();
    auto       texture = Texture::Create(tResource, std::move(gpuImage), tDetails);
    if (nullptr == texture)
    {
        LogError("Texture could not be created from the image");
        return std::numeric_limits<std::size_t>::max();
    }

    textures.push_back(std::move(texture));
    writeBindlessDescriptor(static_cast<TextureIndex>(index), *textures.back());

    return index;
}

auto TextureCache::GetOrCreateTextureIndex(resource::ResourceId const tResource, TextureDetails const& tDetails)
    -> TextureIndex
{
    if (tResource == 0 || tResource == kDefaultTexture)
    {
        return kDefaultTextureIndex;
    }

    for (std::size_t i{0U}; i < mTextures.size(); ++i)
    {
        if (mTextures[i]->name == tResource)
        {
            return static_cast<TextureIndex>(i);
        }
    }

    auto const created = CreateTexture(tResource, tDetails);
    if (created == std::numeric_limits<std::size_t>::max())
    {
        return kDefaultTextureIndex;
    }

    return static_cast<TextureIndex>(created);
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
    return mBindlessSetLayout;
}

auto TextureCache::GetBindlessDescriptorSet() -> VkDescriptorSet
{
    return mBindlessSet;
}

auto TextureCache::writeBindlessDescriptor(TextureIndex const tIndex, Texture const& tTexture) -> void
{
    VkDescriptorImageInfo const imageInfo{
        .sampler     = tTexture.sampler,
        .imageView   = tTexture.view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkWriteDescriptorSet const write{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = mBindlessSet,
        .dstBinding       = 0U,
        .dstArrayElement  = tIndex,
        .descriptorCount  = 1U,
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = &imageInfo,
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(VulkanDevice::Get().logiDevice, 1U, &write, 0U, nullptr);
}

auto TextureCache::Destroy() -> void
{
    VkDevice const device = VulkanDevice::Get().logiDevice;
    for (std::size_t i{0U}; i < mTextures.size(); ++i)
    {
        vkDestroyImageView(device, mTextures[i]->view, nullptr);
        vkDestroySampler(device, mTextures[i]->sampler, nullptr);
    }
    mTextures.clear();

    if (VK_NULL_HANDLE != mBindlessPool)
    {
        vkDestroyDescriptorPool(device, std::exchange(mBindlessPool, VK_NULL_HANDLE), nullptr);
        mBindlessSet = VK_NULL_HANDLE;
    }
    if (VK_NULL_HANDLE != mBindlessSetLayout)
    {
        vkDestroyDescriptorSetLayout(device, std::exchange(mBindlessSetLayout, VK_NULL_HANDLE), nullptr);
    }
}

}// namespace polos::rendering
