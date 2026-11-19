///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/resources/material.hpp"

#include "polos/rendering/cache/texture_cache.hpp"

#include <glm/glm.hpp>

namespace polos::rendering
{
auto Material::Create(resource::material::Material const& tMat) -> Material
{
    glm::vec4 baseColorFactor{1.0F};
    for (uint32_t i{0U}; i < tMat.baseColorFactor.size(); ++i)
    {
        baseColorFactor[static_cast<int32_t>(i)] = tMat.baseColorFactor[i];
    }

    auto const& texture = tMat.baseColorTexture;
    return {
        .baseColorFactor  = baseColorFactor,
        .baseColorTexture = TextureCache::Get().GetOrCreateTextureIndex(texture.img, texture.textureDetails),
        .tilingFactor     = tMat.tilingFactor,
    };
}
}// namespace polos::rendering
