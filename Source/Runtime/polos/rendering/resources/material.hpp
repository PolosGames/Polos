///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RESOURCES_MATERIAL_HPP
#define POLOS_RENDERING_RESOURCES_MATERIAL_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/common.hpp"
#include "polos/resource/material.hpp"

#include <glm/glm.hpp>

namespace polos::rendering
{
struct POLOS_API Material
{
    glm::vec4    baseColorFactor{1.0F};
    TextureIndex baseColorTexture{kDefaultTextureIndex};

    [[nodiscard]] static auto Create(resource::material::Material const& tMat) -> Material;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_RESOURCES_MATERIAL_HPP
