///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SSBO_MESH_INSTANCE_HPP
#define POLOS_RENDERING_SSBO_MESH_INSTANCE_HPP

#include <glm/glm.hpp>

#include <cstdint>
#include <limits>

namespace polos::rendering
{

inline constexpr uint32_t kRootParentId{0U};
inline constexpr uint32_t kInvalidMeshId{std::numeric_limits<uint32_t>::max()};

struct alignas(16) MeshInstance
{
    glm::vec3 position{0.0F};
    uint32_t  parentId{kRootParentId};
    glm::vec4 rotation{0.0F, 0.0F, 0.0F, 1.0F};// quat (x, y, z, w)
    float     scale{1.0F};
    uint32_t  meshId{kInvalidMeshId};
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_SSBO_MESH_INSTANCE_HPP
