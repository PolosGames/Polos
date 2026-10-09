///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_MODEL_HPP
#define POLOS_RESOURCE_MODEL_HPP

#include <glm/glm.hpp>
#include <polos/rendering/vertex/primitive_vertex.hpp>

#include <array>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace polos::resource::model
{
// TODO(sorbatdev): Implement some kind of reference system for keeping these values at one place.
struct alignas(64) Vertex// NOLINT
{
    glm::vec3 position{0.0F, 0.0F, 0.0F};
    glm::vec3 normal{0.0F, 0.0F, 0.0F};
    glm::vec3 color{0.0F, 0.0F, 0.0F};
    glm::vec2 texCoord{0.0F, 0.0F};
};

struct Material
{
    std::array<double, 4U> baseColor{0.0, 0.0, 0.0, 1.0};
    double                 metallic{1.0};
    double                 roughness{1.0};
    // int32_t               baseColorTexture; /* texture index or -1 */
    // int32_t               alphaMode;        /* 0 = OPAQUE, 1 = MASK, 2 = BLEND */
    // float                 alphaCutoff;
    // int32_t               doubleSided;
};

struct Primitive
{
    std::size_t attributes{std::numeric_limits<std::size_t>::max()};
    // Index into Model::indexStreams; the sentinel denotes a non-indexed primitive.
    std::size_t indices{std::numeric_limits<std::size_t>::max()};
    Material    material;
};

struct Mesh
{
    std::string            name;
    std::vector<Primitive> primitives;
};

struct Stream
{ std::vector<rendering::PrimitiveVertex> data; };

struct IndexStream
{ std::vector<std::uint32_t> data; };

struct Model
{
    std::string              name;
    std::vector<Stream>      streams;
    std::vector<IndexStream> indexStreams;
    std::vector<Mesh>        meshes;
};
}// namespace polos::resource::model

#endif// POLOS_RESOURCE_MODEL_HPP
