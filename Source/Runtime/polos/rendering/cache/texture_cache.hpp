///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_CACHE_TEXTURE_CACHE_HPP
#define POLOS_RENDERING_CACHE_TEXTURE_CACHE_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/resources/texture.hpp"
#include "polos/rendering/vk/descriptor_pool.hpp"
#include "polos/resource/resource_manager.hpp"
#include "polos/utils/error_code.hpp"

#include <vulkan/vulkan.h>

#include <vector>

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

    /// @brief Create texture with given sampler details and receive the index
    /// @param tResource ResourceId of the texture that is already in resource catalogue
    /// @param tDetails info that'll be used when creating the sampler
    /// @return Index of the texure inside the cache.
    [[nodiscard]] auto CreateTexture(resource::ResourceId tResource, SamplerDetails const& tDetails) -> uint32_t;

    [[nodiscard]] auto GetOrCreateTextureIndex(resource::ResourceId tResource, SamplerDetails const& tDetails = {})
        -> TextureIndex;

    [[nodiscard]] auto GetTexture(resource::ResourceId tResource) -> Texture const*;

    [[nodiscard]] auto GetTextureAtIndex(std::size_t tIndex) -> Texture const*;

    [[nodiscard]] auto GetBindlessSetLayout() -> VkDescriptorSetLayout;
    [[nodiscard]] auto GetBindlessDescriptorSet() -> VkDescriptorSet;
private:
    friend class RenderCompositor;

    auto createTexture() -> bool;
    auto loadTexturesFromResources() -> bool;
    auto writeBindlessDescriptor(TextureIndex tIndex, Texture const& tTexture) -> void;

    static TextureCache* sInstance;

    std::vector<std::unique_ptr<Texture>> mTextures;

    DescriptorPool mBindlessPool;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_CACHE_TEXTURE_CACHE_HPP
