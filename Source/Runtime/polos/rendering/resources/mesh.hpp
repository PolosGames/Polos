///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RESOURCES_MESH_HPP
#define POLOS_RENDERING_RESOURCES_MESH_HPP

#include "polos/rendering/common.hpp"
#include "polos/rendering/vertex/mesh_vertex.hpp"

#include <optional>
#include <string>
#include <vector>

namespace polos::rendering
{

struct AABB
{
    glm::vec3 max;
    glm::vec3 min;
};

struct MeshPrimitiveData
{
    std::string                name;
    std::vector<MeshVertex>    primitiveVertices{};
    std::vector<std::uint32_t> primitiveIndices{};
    ResourceId                 material{kDefaultMaterial};
    std::optional<AABB>        bounds{};
};

struct MeshPrimitive
{
    ResourceId name;
    MeshIndex  index;
    ResourceId material;
    AABB       bounds;
};

struct Mesh
{
    std::string             name;
    std::vector<ResourceId> primitives;
    AABB                    bounds;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RESOURCES_MESH_HPP
