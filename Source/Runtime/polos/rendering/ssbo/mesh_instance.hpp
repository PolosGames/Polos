///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SSBO_MESH_INSTANCE_HPP
#define POLOS_RENDERING_SSBO_MESH_INSTANCE_HPP

#include "polos/rendering/common.hpp"

#include <glm/glm.hpp>

#include <cstdint>

namespace polos::rendering
{

struct alignas(16) MeshInstance
{
    glm::vec3    position{0.0F};
    uint32_t     parentId{kRootParentId};
    glm::vec4    rotation{0.0F, 0.0F, 0.0F, 1.0F};// quat (x, y, z, w)
    float        scale{1.0F};
    MeshIndex    meshId{kInvalidMeshId};
    TextureIndex texID{kDefaultTextureIndex};
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_SSBO_MESH_INSTANCE_HPP
