///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_MATERIAL_HPP
#define POLOS_RESOURCE_MATERIAL_HPP

#include "polos/rendering/common.hpp"
#include "polos/rendering/resources/texture.hpp"
#include "polos/resource/common.hpp"

#include <array>
#include <cstdint>

namespace polos::resource::material
{
enum class AlphaMode : uint8_t
{
    kOpaque,
    kMask,
    kBlend,
};

struct MaterialTexture
{
    ResourceId                img{0};
    rendering::SamplerDetails textureDetails{};
};

struct Material
{
    ResourceId            name;
    std::array<float, 4U> baseColorFactor;
    MaterialTexture       baseColorTexture;
    float                 tilingFactor{rendering::kDefaultTilingFactor};

    // std::array<float, 3U>    emissiveFactor{0.0F, 0.0F, 0.0F};
    // AlphaMode                alphaMode = AlphaMode::kOpaque;
    // float                    alphaCutoff{0.5F};
    // float                    metallicFactor{1.0F};
    // float                    roughnessFactor{1.0F};
    // rendering::TextureHandle metallicRoughnessTexture = rendering::kDefaultTexture;
    // rendering::TextureHandle normalTexture            = rendering::kDefaultTexture;
    // rendering::TextureHandle occlusionTexture         = rendering::kDefaultTexture;
    // rendering::TextureHandle emissiveTexture          = rendering::kDefaultTexture;
};
}// namespace polos::resource::material

#endif// POLOS_RESOURCE_MATERIAL_HPP
