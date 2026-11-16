///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RESOURCES_MATERIAL_HPP
#define POLOS_RENDERING_RESOURCES_MATERIAL_HPP

#include "polos/rendering/common.hpp"

#include <glm/glm.hpp>

namespace polos::rendering
{
struct Material
{
    glm::vec4    baseColorFactor{1.0F};
    TextureIndex baseColorTexture{kDefaultTextureIndex};
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_RESOURCES_MATERIAL_HPP
