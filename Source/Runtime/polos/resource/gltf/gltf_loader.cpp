///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/gltf/gltf_loader.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/resource/model.hpp"
#include "polos/resource/resource_manager.hpp"
#include "polos/utils/constexpr_map.hpp"
#include "polos/utils/pair.hpp"
#include "polos/utils/string_id.hpp"

#include <tiny_gltf_v3.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace polos::resource::model
{

namespace
{

auto ToSv(tg3_str const& tStr) -> std::string_view
{
    return tStr.len == 0 ? std::string_view{} : std::string_view(tStr.data, tStr.len);
}

using FilterPair = utils::Pair<VkFilter, VkSamplerMipmapMode>;
auto FindFilterPair(int32_t const tFilter) -> FilterPair
{
    switch (tFilter)
    {
        case TG3_TEXTURE_FILTER_NEAREST: return {.first = VK_FILTER_NEAREST, .second = VK_SAMPLER_MIPMAP_MODE_NEAREST};
        case TG3_TEXTURE_FILTER_LINEAR: return {.first = VK_FILTER_LINEAR, .second = VK_SAMPLER_MIPMAP_MODE_LINEAR};
        case TG3_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
            return {.first = VK_FILTER_NEAREST, .second = VK_SAMPLER_MIPMAP_MODE_NEAREST};
        case TG3_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
            return {.first = VK_FILTER_LINEAR, .second = VK_SAMPLER_MIPMAP_MODE_NEAREST};
        case TG3_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
            return {.first = VK_FILTER_NEAREST, .second = VK_SAMPLER_MIPMAP_MODE_LINEAR};
        case TG3_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
        default: return {.first = VK_FILTER_LINEAR, .second = VK_SAMPLER_MIPMAP_MODE_LINEAR};
    }
}

auto FindTextureWrap(int32_t tWrap) -> VkSamplerAddressMode
{
    switch (tWrap)
    {
        case TG3_TEXTURE_WRAP_CLAMP_TO_EDGE: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case TG3_TEXTURE_WRAP_MIRRORED_REPEAT: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case TG3_TEXTURE_WRAP_REPEAT:
        default: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

using namespace std::string_view_literals;
constexpr utils::ConstexprMap kAlphaModeLookup(
    std::array<utils::Pair<std::string_view, AlphaMode>, 3U>{
        utils::MakePair("OPAQUE"sv, AlphaMode::kOpaque),
        utils::MakePair("MASK"sv, AlphaMode::kMask),
        utils::MakePair("BLEND"sv, AlphaMode::kBlend),
    });

auto CheckBufferSources(tg3_model const& tGltf) -> bool
{
    for (auto const& buffer : std::span(tGltf.buffers, tGltf.buffers_count))
    {
        if (buffer.data.data == nullptr || buffer.data.count == 0)
        {
            LogError("glTF buffer data is missing.");
            return false;
        }
    }

    for (auto const& accessor : std::span(tGltf.accessors, tGltf.accessors_count))
    {
        if (accessor.sparse.is_sparse || accessor.buffer_view < 0)
        {
            LogError("Sparse accessors and accessors without a buffer view are not supported.");
            return false;
        }
    }

    return true;
}

auto LoadTextures(tg3_model const& tGltf, std::filesystem::path const& tDirectory, std::span<Texture> tTextures) -> bool
{
    for (std::size_t i{0}; i < tTextures.size(); ++i)
    {
        auto const& texture = tGltf.textures[i];
        if (texture.source < 0)
        {
            continue;
        }

        auto const& image = tGltf.images[texture.source];
        if (image.buffer_view >= 0)
        {
            LogError("Images stored in buffer views are not supported.");
            return false;
        }

        auto const        imagePath = tDirectory / std::filesystem::path(ToSv(image.uri));
        auto&             loaded    = tTextures[i];
        std::string const hashStr   = "texture." + imagePath.stem().string();

        loaded.resourceId = utils::StrHash64Runtime(hashStr);

        if (texture.sampler >= 0)
        {
            auto const& sampler   = tGltf.samplers[texture.sampler];
            auto const  minFilter = FindFilterPair(sampler.min_filter);
            loaded.samplerDetails = {
                .minFilter    = minFilter.first,
                .magFilter    = FindFilterPair(sampler.mag_filter).first,
                .mipmap       = minFilter.second,
                .addressModeU = FindTextureWrap(sampler.wrap_s),
                .addressModeV = FindTextureWrap(sampler.wrap_t),
            };
        }
        loaded.path = imagePath;
    }

    return true;
}

auto LoadMaterials(tg3_model const& tGltf) -> std::vector<Material>
{
    auto const textureOrDefault = [](std::int32_t tIndex) {
        return tIndex >= 0 ? static_cast<std::size_t>(tIndex) : kNoTextureIndex;
    };
    auto const toFloat = [](double tValue) {
        return static_cast<float>(tValue);
    };

    std::vector<Material> materials(tGltf.materials_count);
    for (std::size_t i{0}; i < materials.size(); ++i)
    {
        auto const& source   = tGltf.materials[i];
        auto const& pbr      = source.pbr_metallic_roughness;
        auto&       material = materials[i];

        material.name = ToSv(source.name);
        std::ranges::transform(source.emissive_factor, material.emissiveFactor.begin(), toFloat);
        std::ranges::transform(pbr.base_color_factor, material.baseColorFactor.begin(), toFloat);
        material.alphaMode                = *kAlphaModeLookup.at(ToSv(source.alpha_mode));
        material.alphaCutoff              = static_cast<float>(source.alpha_cutoff);
        material.baseColorTexture         = textureOrDefault(pbr.base_color_texture.index);
        material.metallicFactor           = static_cast<float>(pbr.metallic_factor);
        material.roughnessFactor          = static_cast<float>(pbr.roughness_factor);
        material.metallicRoughnessTexture = textureOrDefault(pbr.metallic_roughness_texture.index);
        material.normalTexture            = textureOrDefault(source.normal_texture.index);
        material.occlusionTexture         = textureOrDefault(source.occlusion_texture.index);
        material.emissiveTexture          = textureOrDefault(source.emissive_texture.index);
    }

    return materials;
}

auto LogParseErrors(tinygltf3::ErrorStack const& tErrors) -> void
{
    for (std::uint32_t i{0}; i < tErrors.count(); ++i)
    {
        tg3_error_entry const* err = tErrors.entry(i);
        LogError("[{}] {}", static_cast<std::uint32_t>(err->severity), err->message ? err->message : "(null)");
    }
}

struct AccessorData
{
    std::uint8_t const* bytes;
    std::size_t         stride;
};

auto GetAccessorData(tg3_model const& tGltf, tg3_accessor const& tAccessor) -> AccessorData
{
    // tinygltf has already checked the buffer references and byte ranges.
    auto const& view   = tGltf.buffer_views[tAccessor.buffer_view];
    auto const& buffer = tGltf.buffers[view.buffer];
    return {
        .bytes  = buffer.data.data + view.byte_offset + tAccessor.byte_offset,
        .stride = static_cast<std::size_t>(tg3_accessor_byte_stride(&tAccessor, &view)),
    };
}

// glTF components are little-endian and may be unaligned.
auto ReadUnsigned(std::uint8_t const* tBytes, std::size_t tSize) -> std::uint32_t
{
    std::uint32_t value{0};
    for (std::size_t i{0}; i < tSize; ++i)
    {
        value |= static_cast<std::uint32_t>(tBytes[i]) << (i * 8U);
    }
    return value;
}

auto FindAttribute(tg3_model const& tGltf, std::span<tg3_str_int_pair const> tAttributes, std::string_view tName)
    -> tg3_accessor const*
{
    auto const attribute = std::ranges::find(tAttributes, tName, [](tg3_str_int_pair const& tAttribute) {
        return ToSv(tAttribute.key);
    });
    return attribute == tAttributes.end() ? nullptr : &tGltf.accessors[attribute->value];
}

template<typename Vector>
auto LoadAttribute(
    tg3_model const&                      tGltf,
    tg3_accessor const*                   tAccessor,
    std::span<rendering::PrimitiveVertex> tVertices,
    Vector rendering::PrimitiveVertex::* tMember) -> bool
{
    if (tAccessor == nullptr)
    {
        return true;
    }

    auto const componentCount = tg3_num_components(tAccessor->type);
    if (tAccessor->count != tVertices.size() || componentCount > Vector::length())
    {
        LogError("Vertex attribute does not fit the mesh vertex layout.");
        return false;
    }

    auto const [bytes, stride] = GetAccessorData(tGltf, *tAccessor);
    auto const componentSize   = static_cast<std::size_t>(tg3_component_size(tAccessor->component_type));
    auto const isFloat         = tAccessor->component_type == TG3_COMPONENT_TYPE_FLOAT;
    auto const normalizedScale = componentSize == 1U ? 255.0F : 65535.0F;

    for (std::size_t i{0}; i < tVertices.size(); ++i)
    {
        auto&       attribute      = tVertices[i].*tMember;
        auto const* componentBytes = bytes + (i * stride);
        for (glm::length_t component{0}; component < componentCount; ++component)
        {
            auto const value     = ReadUnsigned(componentBytes, componentSize);
            attribute[component] = isFloat ? std::bit_cast<float>(value) : static_cast<float>(value) / normalizedScale;
            componentBytes += componentSize;
        }
    }
    return true;
}

auto LoadVertices(
    tg3_model const&                         tGltf,
    tg3_primitive const&                     tPrimitive,
    std::vector<rendering::PrimitiveVertex>& tVertices) -> bool
{
    using rendering::PrimitiveVertex;

    auto const  attributes = std::span(tPrimitive.attributes, tPrimitive.attributes_count);
    auto const* position   = FindAttribute(tGltf, attributes, "POSITION");
    if (position == nullptr)
    {
        LogError("Mesh primitive is missing POSITION.");
        return false;
    }

    tVertices.resize(static_cast<std::size_t>(position->count));
    auto const loadAttribute = [&](std::string_view tName, auto tMember) {
        return LoadAttribute(tGltf, FindAttribute(tGltf, attributes, tName), tVertices, tMember);
    };

    if (!LoadAttribute(tGltf, position, tVertices, &PrimitiveVertex::position) ||
        !loadAttribute("NORMAL", &PrimitiveVertex::normal) || !loadAttribute("TANGENT", &PrimitiveVertex::tangent) ||
        !loadAttribute("TEXCOORD_0", &PrimitiveVertex::texCoord) || !loadAttribute("COLOR_0", &PrimitiveVertex::color))
    {
        return false;
    }

    // stbi_set_flip_vertically_on_load_thread
    for (auto& vertex : tVertices)
    {
        vertex.texCoord.y = 1.0F - vertex.texCoord.y;
    }

    return true;
}

auto LoadIndices(tg3_model const& tGltf, tg3_accessor const& tAccessor, std::vector<std::uint32_t>& tIndices) -> void
{
    auto const [bytes, stride] = GetAccessorData(tGltf, tAccessor);
    auto const componentSize   = static_cast<std::size_t>(tg3_component_size(tAccessor.component_type));
    tIndices.resize(static_cast<std::size_t>(tAccessor.count));
    for (std::size_t i{0}; i < tIndices.size(); ++i)
    {
        tIndices[i] = ReadUnsigned(bytes + i * stride, componentSize);
    }
}

auto LoadMeshParts(tg3_model const& tGltf, std::span<Material const> tMaterials, std::size_t tMeshIndex, Mesh& tMesh)
    -> bool
{
    auto const& source = tGltf.meshes[tMeshIndex];
    tMesh.name         = ToSv(source.name);
    tMesh.parts.resize(source.primitives_count);

    auto const partPrefix = "mesh." + (tMesh.name.empty() ? std::to_string(tMeshIndex) : tMesh.name) + ".part.";
    for (std::size_t i{0}; i < tMesh.parts.size(); ++i)
    {
        auto const& primitive = source.primitives[i];
        if (primitive.mode != TG3_MODE_TRIANGLES)
        {
            LogError("Only triangle-list mesh primitives are supported.");
            return false;
        }

        auto& part = tMesh.parts[i];
        part.name  = utils::StrHash64Runtime(partPrefix + std::to_string(i));
        if (primitive.material >= 0)
        {
            part.material = tMaterials[static_cast<std::size_t>(primitive.material)];
        }

        if (!LoadVertices(tGltf, primitive, part.primitiveVertices))
        {
            return false;
        }

        if (primitive.indices >= 0)
        {
            LoadIndices(tGltf, tGltf.accessors[primitive.indices], part.primitiveIndices);
        }
    }

    return true;
}

auto LoadMeshes(tg3_model const& tGltf, std::span<Material const> tMaterials, std::span<Mesh> tMeshes) -> bool
{
    for (std::size_t i{0}; i < tMeshes.size(); ++i)
    {
        if (!LoadMeshParts(tGltf, tMaterials, i, tMeshes[i]))
        {
            return false;
        }
    }
    return true;
}

}// namespace

auto LoadGltf(std::filesystem::path const& tPath) -> std::unique_ptr<Model>
{
    tinygltf3::Model      tgModel;
    tinygltf3::ErrorStack tgErrors;
    tg3_parse_options     tgOpts;

    tg3_parse_options_init(&tgOpts);
    tgOpts.strictness       = TG3_STRICT;
    tgOpts.parse_float32    = 0;
    tgOpts.validate_indices = 1;
    tgOpts.validate_ranges  = 1;

    auto const path = tPath.string();
    auto const parseResult =
        tg3_parse_file(tgModel.get(), tgErrors.get(), path.c_str(), static_cast<std::uint32_t>(path.size()), &tgOpts);
    if (parseResult != TG3_OK)
    {
        LogParseErrors(tgErrors);
        return nullptr;
    }

    auto model  = std::make_unique<Model>();
    model->name = tPath.stem().string();

    auto const& gltf = *tgModel.get();
    model->textures.resize(gltf.textures_count);
    if (!CheckBufferSources(gltf) || !LoadTextures(gltf, tPath.parent_path(), model->textures))
    {
        return nullptr;
    }

    auto const materials = LoadMaterials(gltf);

    model->meshes.resize(gltf.meshes_count);
    if (!LoadMeshes(gltf, materials, model->meshes))
    {
        return nullptr;
    }
    return model;
}

}// namespace polos::resource::model
