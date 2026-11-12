///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RESOURCES_MATERIAL_HPP
#define POLOS_RENDERING_RESOURCES_MATERIAL_HPP

#include "polos/rendering/common.hpp"
#include "polos/utils/string_id.hpp"

namespace polos::rendering
{

enum class AlphaMode : uint8_t
{
    kOpaque,
    kMask,
    kBlend,
};

struct Material
{
    utils::string_id      name;
    std::array<float, 3U> emissiveFactor{0.0F, 0.0F, 0.0F};
    AlphaMode             alphaMode{AlphaMode::kOpaque};
    float                 alphaCutoff{0.5F};
    std::array<float, 4U> baseColorFactor{1.0F, 1.0F, 1.0F, 1.0F};
    TextureIndex          baseColorTexture{kDefaultTextureIndex};
    float                 metallicFactor{1.0F};
    float                 roughnessFactor{1.0F};
    TextureIndex          metallicRoughnessTexture{kDefaultTextureIndex};
    TextureIndex          normalTexture{kDefaultTextureIndex};
    TextureIndex          occlusionTexture{kDefaultTextureIndex};
    TextureIndex          emissiveTexture{kDefaultTextureIndex};

    auto operator==(Material const&) const -> bool = default;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_RESOURCES_MATERIAL_HPP
