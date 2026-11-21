///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_MODEL_HPP
#define POLOS_RESOURCE_MODEL_HPP

#include "polos/rendering/resources/texture.hpp"
#include "polos/rendering/vertex/mesh_vertex.hpp"
#include "polos/resource/common.hpp"
#include "polos/resource/material.hpp"
#include "polos/utils/pair.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
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
    std::optional<AABB>                bounds;
};

struct Mesh
{
    std::string                name;
    std::vector<MeshPrimitive> primitives;
    AABB                       bounds;
};

struct Node
{
    std::string                name;
    glm::vec3                  position{0.0F};
    glm::quat                  rotation{1.0F, 0.0F, 0.0F, 0.0F};
    float                      scale{1.0F};
    std::optional<std::size_t> meshIndex;
    std::vector<std::size_t>   children;
};

struct Model
{
    std::string                                               name;
    std::vector<Mesh>                                         meshes;
    std::vector<Node>                                         nodes;
    std::vector<std::size_t>                                  rootNodes;
    std::unordered_map<ResourceId, material::MaterialTexture> materialTextures;
};
}// namespace polos::resource::model

#endif// POLOS_RESOURCE_MODEL_HPP
