///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_CACHE_TEXTURE_CACHE_HPP
#define POLOS_RENDERING_CACHE_TEXTURE_CACHE_HPP

#include "polos/rendering/resources/texture.hpp"
#include "polos/resource/resource_manager.hpp"
#include "polos/utils/error_code.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace polos::rendering
{

class TextureCache
{
public:
    struct CreationDetails
    {
    };

    TextureCache();
    ~TextureCache();

    auto Create(CreationDetails const&) -> Result<void>;
    auto Destroy() -> void;

    /// @brief Create texture with given sampler details and receive the index
    /// @param tResource ResourceId of the texture that is already in resource catalogue
    /// @param tDetails info that'll be used when creating the sampler
    /// @return Index of the texure inside the cache.
    [[nodiscard]] static auto CreateTexture(resource::ResourceId tResource, TextureDetails const& tDetails)
        -> std::size_t;

    [[nodiscard]] static auto GetTexture(resource::ResourceId tResource) -> Texture const*;

    [[nodiscard]] static auto GetTextureAtIndex(std::size_t tIndex) -> Texture const*;
private:
    friend class RenderCompositor;

    auto createTexture() -> bool;
    auto loadTexturesFromResources() -> bool;

    static TextureCache* sInstance;

    std::vector<std::unique_ptr<Texture>> mTextures;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_CACHE_TEXTURE_CACHE_HPP
