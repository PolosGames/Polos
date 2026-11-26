///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_CACHE_TEXTURE_CACHE_HPP
#define POLOS_RENDERING_CACHE_TEXTURE_CACHE_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/resources/texture.hpp"
#include "polos/rendering/vk/descriptor_pool.hpp"
#include "polos/utils/error_code.hpp"

#include <vulkan/vulkan.h>

#include <unordered_map>

namespace polos::rendering
{

class POLOS_API TextureCache
{
public:
    [[nodiscard]] static auto Get() -> TextureCache&;

    struct CreationDetails
    {
    };

    TextureCache();
    ~TextureCache();

    TextureCache(TextureCache const&)                    = delete;
    auto operator=(TextureCache const&) -> TextureCache& = delete;
    TextureCache(TextureCache&&)                         = delete;
    auto operator=(TextureCache&&) -> TextureCache&      = delete;

    auto Create(CreationDetails const&) -> Result<void>;
    auto Destroy() -> void;

    [[nodiscard]] auto AddTexture(ResourceId tId, VkExtent2D tExtent, std::span<std::uint8_t const> tPixels, SamplerDetails const& tDetails) -> bool;
    [[nodiscard]] auto GetTexture(ResourceId tId) const -> Texture const*;
    [[nodiscard]] auto GetTextureIndex(ResourceId tId) const -> TextureIndex;

    [[nodiscard]] auto GetBindlessSetLayout() -> VkDescriptorSetLayout;
    [[nodiscard]] auto GetBindlessDescriptorSet() -> VkDescriptorSet;
private:
    friend class RenderCompositor;

    auto writeBindlessDescriptor(TextureIndex tIndex, Texture const& tTexture) -> void;

    static TextureCache* sInstance;

    std::unordered_map<ResourceId, std::unique_ptr<Texture>> mTextures;

    DescriptorPool mBindlessPool;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_CACHE_TEXTURE_CACHE_HPP
