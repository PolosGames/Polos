///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/gltf/gltf_loader.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/common.hpp"
#include "polos/resource/model.hpp"
#include "polos/utils/constexpr_map.hpp"
#include "polos/utils/pair.hpp"
#include "polos/utils/yaml_read.hpp"

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

namespace polos::resource::model
{

namespace
{
// Reuse the parser across loads on the same worker. Its parse state is thread-local.
thread_local fastgltf::Parser gParser;

using fastgltf::AccessorType;
using fastgltf::ComponentType;
using rendering::MeshVertex;
using namespace std::string_view_literals;

auto HasAccessorData(fastgltf::Asset const& tAsset, fastgltf::Accessor const& tAccessor) -> bool
{
    if (!tAccessor.bufferViewIndex)
    {
        return false;
    }
    if (tAccessor.sparse)
    {
        return false;
    }
    if (fastgltf::validateAccessorBounds(tAsset, tAccessor) != fastgltf::Error::None)
    {
        return false;
    }

    auto const& view = tAsset.bufferViews[*tAccessor.bufferViewIndex];
    auto const& data = tAsset.buffers[view.bufferIndex].data;
    return std::holds_alternative<fastgltf::sources::Array>(data) ||
           std::holds_alternative<fastgltf::sources::Vector>(data) ||
           std::holds_alternative<fastgltf::sources::ByteView>(data);
}

template<auto Member>
using VertexMember = std::integral_constant<decltype(Member), Member>;

template<auto Member, typename Element>
auto ReadAttribute(fastgltf::Asset const& tAsset, fastgltf::Accessor const& tAccessor, std::span<MeshVertex> tVertices)
    -> bool
{
    if (tAccessor.type != fastgltf::ElementTraits<Element>::type)
    {
        return false;
    }

    constexpr bool isTexCoord = std::is_same_v<VertexMember<Member>, VertexMember<&MeshVertex::texCoord>>;
    constexpr bool isColor    = std::is_same_v<VertexMember<Member>, VertexMember<&MeshVertex::color>>;
    if (tAccessor.normalized)
    {
        constexpr bool supportsNormalized = isTexCoord || isColor;
        if (!supportsNormalized)
        {
            return false;
        }

        bool const isUnsigned = tAccessor.componentType == ComponentType::UnsignedByte ||
                                tAccessor.componentType == ComponentType::UnsignedShort;
        if (!isUnsigned)
        {
            return false;
        }
    }
    else if (tAccessor.componentType != ComponentType::Float)
    {
        return false;
    }

    fastgltf::iterateAccessorWithIndex<Element>(tAsset, tAccessor, [&](Element tValue, size_t tIndex) {
        if constexpr (isTexCoord)
        {
            tVertices[tIndex].texCoord = {tValue.x, 1.0F - tValue.y};
        }
        else if constexpr (isColor && std::is_same_v<Element, glm::vec3>)
        {
            tVertices[tIndex].color = glm::vec4(tValue, 1.0F);
        }
        else
        {
            tVertices[tIndex].*Member = tValue;
        }
    });
    return true;
}

auto ReadColor(fastgltf::Asset const& tAsset, fastgltf::Accessor const& tAccessor, std::span<MeshVertex> tVertices)
    -> bool
{
    if (tAccessor.type == AccessorType::Vec3)
    {
        return ReadAttribute<&MeshVertex::color, glm::vec3>(tAsset, tAccessor, tVertices);
    }
    return ReadAttribute<&MeshVertex::color, glm::vec4>(tAsset, tAccessor, tVertices);
}

inline constexpr utils::ConstexprMap kAttribReaders(
    std::array{
        utils::Pair("POSITION"sv, &ReadAttribute<&MeshVertex::position, glm::vec3>),
        utils::Pair("NORMAL"sv, &ReadAttribute<&MeshVertex::normal, glm::vec3>),
        utils::Pair("TANGENT"sv, &ReadAttribute<&MeshVertex::tangent, glm::vec4>),
        utils::Pair("TEXCOORD_0"sv, &ReadAttribute<&MeshVertex::texCoord, glm::vec2>),
        utils::Pair("COLOR_0"sv, &ReadColor),
    });

auto ReadIndices(fastgltf::Asset const& tAsset, fastgltf::Primitive const& tPrimitive, MeshPrimitive& tTarget) -> bool
{
    if (!tPrimitive.indicesAccessor)
    {
        return false;
    }
    if (*tPrimitive.indicesAccessor >= tAsset.accessors.size())
    {
        return false;
    }

    auto const& accessor = tAsset.accessors[*tPrimitive.indicesAccessor];
    if (accessor.type != AccessorType::Scalar)
    {
        return false;
    }
    if (accessor.normalized)
    {
        return false;
    }
    if (accessor.count % 3U != 0)
    {
        return false;
    }

    bool const isUnsigned = accessor.componentType == ComponentType::UnsignedByte ||
                            accessor.componentType == ComponentType::UnsignedShort ||
                            accessor.componentType == ComponentType::UnsignedInt;
    if (!isUnsigned)
    {
        return false;
    }
    if (!HasAccessorData(tAsset, accessor))
    {
        return false;
    }

    auto const& view = tAsset.bufferViews[*accessor.bufferViewIndex];
    if (view.byteStride)
    {
        return false;
    }

    tTarget.primitiveIndices.resize(accessor.count);
    fastgltf::copyFromAccessor<std::uint32_t>(tAsset, accessor, tTarget.primitiveIndices.data());
    return std::ranges::all_of(tTarget.primitiveIndices, [&](std::uint32_t tIndex) {
        return tIndex < tTarget.primitiveVertices.size();
    });
}

auto ReadPositionBound(fastgltf::AccessorBoundsArray const& tBound) -> glm::vec3
{
    auto const* values = tBound.data<double>();
    return glm::vec3(static_cast<float>(values[0]), static_cast<float>(values[1]), static_cast<float>(values[2]));
}

auto ToAddressMode(fastgltf::Wrap tWrap) -> VkSamplerAddressMode
{
    switch (tWrap)
    {
        case fastgltf::Wrap::ClampToEdge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case fastgltf::Wrap::MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case fastgltf::Wrap::Repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

auto ReadSampler(fastgltf::Sampler const& tSampler) -> rendering::SamplerDetails
{
    using fastgltf::Filter;
    auto const min = tSampler.minFilter.value_or(Filter::LinearMipMapLinear);
    bool const nearestMin =
        min == Filter::Nearest || min == Filter::NearestMipMapNearest || min == Filter::NearestMipMapLinear;
    bool const nearestMip = min == Filter::NearestMipMapNearest || min == Filter::LinearMipMapNearest;

    return {
        .minFilter    = nearestMin ? VK_FILTER_NEAREST : VK_FILTER_LINEAR,
        .magFilter    = tSampler.magFilter == Filter::Nearest ? VK_FILTER_NEAREST : VK_FILTER_LINEAR,
        .mipmap       = nearestMip ? VK_SAMPLER_MIPMAP_MODE_NEAREST : VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = ToAddressMode(tSampler.wrapS),
        .addressModeV = ToAddressMode(tSampler.wrapT),
    };
}

auto ReadMaterialTextures(fastgltf::Asset const& tAsset, std::span<ResourceId const> tMaterials, Model& tModel) -> bool
{
    for (std::size_t i{0U}; i < tMaterials.size(); ++i)
    {
        auto const& baseColor = tAsset.materials[i].pbrData.baseColorTexture;
        if (!baseColor)
        {
            continue;
        }
        if (baseColor->textureIndex >= tAsset.textures.size())
        {
            return false;
        }

        auto const& texture = tAsset.textures[baseColor->textureIndex];
        if ((texture.imageIndex && *texture.imageIndex >= tAsset.images.size()) ||
            (texture.samplerIndex && *texture.samplerIndex >= tAsset.samplers.size()))
        {
            return false;
        }

        material::MaterialTexture binding;
        if (texture.imageIndex)
        {
            auto const& image = tAsset.images[*texture.imageIndex];
            if (auto const* uri = std::get_if<fastgltf::sources::URI>(&image.data))
            {
                // The preprocessor uses the image file stem for the catalog ID.
                auto const name = std::filesystem::path(std::string_view(uri->uri->path())).stem().string();
                binding.img     = utils::StrHash64Runtime("texture." + name);
            }
        }
        if (texture.samplerIndex)
        {
            binding.textureDetails = ReadSampler(tAsset.samplers[*texture.samplerIndex]);
        }
        tModel.materialTextures.emplace(tMaterials[i], binding);
    }
    return true;
}

auto ProcessMesh(
    fastgltf::Asset const&      tAsset,
    Mesh&                       tTarget,
    fastgltf::Mesh const&       tInput,
    std::span<ResourceId const> tMaterials) -> bool
{
    if (tInput.primitives.empty())
    {
        return false;
    }

    tTarget.primitives.resize(tInput.primitives.size());
    tTarget.bounds.min = glm::vec3(std::numeric_limits<float>::infinity());
    tTarget.bounds.max = -tTarget.bounds.min;

    for (size_t i{0U}; i < tInput.primitives.size(); ++i)
    {
        MeshPrimitive&             meshPrim  = tTarget.primitives[i];
        fastgltf::Primitive const& primitive = tInput.primitives[i];

        if (primitive.type != fastgltf::PrimitiveType::Triangles)
        {
            return false;
        }

        auto const position = primitive.findAttribute("POSITION");
        if (position == primitive.attributes.end())
        {
            return false;
        }
        if (position->accessorIndex >= tAsset.accessors.size())
        {
            return false;
        }

        auto const& positions = tAsset.accessors[position->accessorIndex];
        if (!HasAccessorData(tAsset, positions))
        {
            return false;
        }
        if (!positions.min || !positions.max)
        {
            LogError("Missing POSITION bounds for {}", tTarget.name);
            return false;
        }

        meshPrim.name     = tTarget.name + "." + std::to_string(i);
        meshPrim.material = rendering::kDefaultMaterial;
        if (primitive.materialIndex)
        {
            if (*primitive.materialIndex >= tMaterials.size())
            {
                return false;
            }
            // The preprocessor writes the material IDs in glTF material order.
            meshPrim.material = tMaterials[*primitive.materialIndex];
        }

        meshPrim.primitiveVertices.resize(positions.count);
        for (auto const& attribute : primitive.attributes)
        {
            auto const reader = kAttribReaders[std::string_view(attribute.name)];
            if (!reader)
            {
                continue;
            }
            if (attribute.accessorIndex >= tAsset.accessors.size())
            {
                return false;
            }

            auto const& accessor = tAsset.accessors[attribute.accessorIndex];
            if (accessor.count != positions.count)
            {
                LogError("{} vertex count differs from POSITION for {}", attribute.name, meshPrim.name);
                return false;
            }
            if (!HasAccessorData(tAsset, accessor))
            {
                LogError("Invalid {} accessor data for {}", attribute.name, meshPrim.name);
                return false;
            }
            if (!(*reader)(tAsset, accessor, meshPrim.primitiveVertices))
            {
                LogError("Unsupported {} accessor format for {}", attribute.name, meshPrim.name);
                return false;
            }
        }

        if (!ReadIndices(tAsset, primitive, meshPrim))
        {
            LogError("Invalid index accessor for {}", meshPrim.name);
            return false;
        }

        meshPrim.bounds = AABB{
            .max = ReadPositionBound(*positions.max),
            .min = ReadPositionBound(*positions.min),
        };
        tTarget.bounds.min = glm::min(tTarget.bounds.min, meshPrim.bounds->min);
        tTarget.bounds.max = glm::max(tTarget.bounds.max, meshPrim.bounds->max);
    }

    return true;
}
}// namespace

auto LoadGltf(std::filesystem::path const& tPath) -> std::unique_ptr<Model>
{
    std::ifstream file(tPath);
    if (!file)
    {
        LogError("Could not open model descriptor: {}", tPath.string());
        return nullptr;
    }

    auto const        tree      = utils::yaml::Parse(file);
    auto const        modelYaml = tree.crootref();
    auto const        id        = utils::yaml::Scalar(utils::yaml::Find(modelYaml, "id"));
    std::string const modelName{id.substr(id.find('.') + 1U)};
    // Model descriptors live in Resources/Models. Their paths are relative to Resources.
    auto const path = tPath.parent_path().parent_path() / utils::yaml::Scalar(utils::yaml::Find(modelYaml, "path"));

    auto data = fastgltf::GltfDataBuffer::FromPath(path);
    if (data.has_error())
    {
        LogError("Could not read model file {}: {}", path.string(), fastgltf::getErrorMessage(data.error()));
        return nullptr;
    }

    auto assetRes = gParser.loadGltf(
        data.get(), path.parent_path(),
        fastgltf::Options::LoadExternalBuffers | fastgltf::Options::GenerateMeshIndices);
    if (assetRes.has_error())
    {
        LogError("Could not parse model file {}: {}", path.string(), fastgltf::getErrorMessage(assetRes.error()));
        return nullptr;
    }

    fastgltf::Asset const& asset = assetRes.get();

#if !defined(NDEBUG)
    if (auto res = fastgltf::validate(asset); res != fastgltf::Error::None)
    {
        LogError("Invalid model {}: {}", modelName, fastgltf::getErrorMessage(res));
        return nullptr;
    }
#endif

    std::vector<ResourceId> materials;
    auto const              matSeq = utils::yaml::Find(modelYaml, "materials");
    if (matSeq.readable() && matSeq.is_seq())
    {
        materials.reserve(matSeq.num_children());
        for (auto material : matSeq.children())
        {
            materials.push_back(utils::StrHash64Runtime(utils::yaml::Scalar(utils::yaml::Find(material, "id"))));
        }
    }

    auto model  = std::make_unique<Model>();
    model->name = modelName;
    if (materials.size() > asset.materials.size() || !ReadMaterialTextures(asset, materials, *model))
    {
        LogError("Invalid material texture references in {}", modelName);
        return nullptr;
    }
    model->meshes.resize(asset.meshes.size());

    for (size_t i{0U}; i < asset.meshes.size(); ++i)
    {
        auto const& input = asset.meshes[i];
        auto&       mesh  = model->meshes[i];
        mesh.name = input.name.empty() ? "mesh." + modelName + std::to_string(i) : "mesh." + std::string(input.name);

        if (!ProcessMesh(asset, mesh, input, materials))
        {
            LogError("Could not process mesh {} in {}", mesh.name, modelName);
            return nullptr;
        }
    }

    return model;
}

}// namespace polos::resource::model
