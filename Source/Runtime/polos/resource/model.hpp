///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_MODEL_HPP
#define POLOS_RESOURCE_MODEL_HPP

#include "polos/rendering/common.hpp"
#include "polos/rendering/resources/texture.hpp"
#include "polos/rendering/vertex/primitive_vertex.hpp"
#include "polos/utils/string_id.hpp"

#include <glm/glm.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace polos::resource::model
{

struct Texture
{
    utils::string_id          resourceId{rendering::kDefaultTexture};
    rendering::TextureDetails samplerDetails{};
};

enum class AlphaMode : uint8_t
{
    kOpaque,
    kMask,
    kBlend,
};

// TODO(sorbatdev): Implement some kind of reference system for keeping these values at one place.
struct Material
{
    // int32_t               doubleSided;                   /* 0 or 1 */
    // const int32_t*        lods;
    // uint32_t              lods_count;
    std::array<float, 3U> emissiveFactor{0.0F, 0.0F, 0.0F};
    AlphaMode             alphaMode{AlphaMode::kOpaque};
    float                 alphaCutoff{0.5F};
    std::array<float, 4U> baseColorFactor{1.0F, 1.0F, 1.0F, 1.0F};
    Texture               baseColorTexture{};
    float                 metallicFactor{1.0F};
    float                 roughnessFactor{1.0F};
    Texture               metallicRoughnessTexture;
    Texture               normalTexture;
    Texture               occlusionTexture;
    Texture               emissiveTexture;
};

struct MeshPart
{
    utils::string_id                        name;
    std::vector<rendering::PrimitiveVertex> primitiveVertices;
    std::vector<uint32_t>                   primitiveIndices;
    Material                                material;
};

struct Mesh
{
    std::string           name;
    std::vector<MeshPart> parts;
};

struct Model
{
    std::string       name;
    std::vector<Mesh> meshes;
};
}// namespace polos::resource::model

#endif// POLOS_RESOURCE_MODEL_HPP
