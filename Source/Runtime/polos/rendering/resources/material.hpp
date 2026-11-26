///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RESOURCES_MATERIAL_HPP
#define POLOS_RENDERING_RESOURCES_MATERIAL_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/common.hpp"

#include <glm/glm.hpp>

namespace polos::rendering
{
struct POLOS_API Material
{
    glm::vec4  baseColorFactor{1.0F};
    ResourceId baseColorTexture{kDefaultTexture};
    float      tilingFactor{kDefaultTilingFactor};
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_RESOURCES_MATERIAL_HPP
