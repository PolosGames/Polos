///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_MODEL_HPP
#define POLOS_RESOURCE_MODEL_HPP

#include "polos/rendering/resources/texture.hpp"
#include "polos/rendering/vertex/mesh_vertex.hpp"
#include "polos/resource/common.hpp"
#include "polos/utils/pair.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace polos::resource::model
{

struct AABB
{
    glm::vec3 max;
    glm::vec3 min;
};

struct MeshPrimitive
{
    std::string                        name;
    std::vector<rendering::MeshVertex> primitiveVertices;
    std::vector<uint32_t>              primitiveIndices;
    ResourceId                         material;
};

struct Mesh
{
    std::string                name;
    std::vector<MeshPrimitive> parts;
    AABB                       bounds;
};

struct Model
{
    std::string       name;
    std::vector<Mesh> meshes;
};
}// namespace polos::resource::model

#endif// POLOS_RESOURCE_MODEL_HPP
