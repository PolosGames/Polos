///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/gltf/gltf_loader.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/resource/model.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/constexpr_map.hpp"
#include "polos/utils/pair.hpp"

#include <fastgltf/core.hpp>
#include <yaml-cpp/yaml.h>

namespace polos::resource::model
{

namespace
{
thread_local fastgltf::Parser gParser;

struct AttribInfo
{
    size_t offset{0U};
    size_t size{0U};
};

using namespace std::string_view_literals;
// inline constexpr std::array kAttribInfo{
// };

inline constexpr utils::ConstexprMap kAttribInfos(
    std::array{
        utils::Pair(
            "POSITION"sv,
            AttribInfo{
                .offset = offsetof(rendering::MeshVertex, position),
                .size   = sizeof(rendering::MeshVertex::position),
            }),
        utils::Pair(
            "NORMAL"sv,
            AttribInfo{
                .offset = offsetof(rendering::MeshVertex, normal),
                .size   = sizeof(rendering::MeshVertex::normal),
            }),
        utils::Pair(
            "TANGENT"sv,
            AttribInfo{
                .offset = offsetof(rendering::MeshVertex, tangent),
                .size   = sizeof(rendering::MeshVertex::tangent),
            }),
        utils::Pair(
            "TEXCOORD_0"sv,
            AttribInfo{
                .offset = offsetof(rendering::MeshVertex, texCoord),
                .size   = sizeof(rendering::MeshVertex::texCoord),
            }),
        utils::Pair(
            "COLOR_0"sv,
            AttribInfo{
                .offset = offsetof(rendering::MeshVertex, color),
                .size   = sizeof(rendering::MeshVertex::color),
            }),
    });

}// namespace

// MeshPrimitive(s) == Primitive(s)
auto ProcessMesh(
    fastgltf::Asset const& tAsset,
    Mesh&                  tTarget,
    fastgltf::Mesh const&  tInput,
    std::span<ResourceId>  tMaterials) -> bool
{
    tTarget.parts.resize(tInput.primitives.size());

    for (size_t i{0U}; i < tInput.primitives.size(); ++i)
    {
        MeshPrimitive&             meshPrim  = tTarget.parts[i];
        fastgltf::Primitive const& primitive = tInput.primitives[i];

        if (primitive.materialIndex.has_value())
        {
            // Material order will be in the same order they'll be declared in the gltf file,
            // Because we pre-process them in the same order they appear.
            meshPrim.material = tMaterials[primitive.materialIndex.value()];
        }

        for (auto const& attribute : primitive.attributes)
        {
            // TODO(sorbatdev): Sparse accessors
            auto const& accessor = tAsset.accessors[attribute.accessorIndex];

            auto attribRes = kAttribInfos[std::string_view(attribute.name)];
            if (!attribRes.has_value())
            {
                LogDebug("Unsupported attribute: {}", attribute.name);
                continue;
            }

            size_t const vertexOffset      = attribRes->offset;
            size_t const supportedCompSize = attribRes->size;
        }
    }

    return true;
}

auto LoadGltf(std::filesystem::path const& tPath) -> std::unique_ptr<Model>
{
    YAML::Node modelYaml = YAML::LoadFile(tPath.string());

    auto const&                 modelName = modelYaml["id"].as<std::string>();
    std::filesystem::path const path      = modelYaml["path"].as<std::string>();

    auto data = fastgltf::GltfDataBuffer::FromPath(path);
    if (data.has_error())
    {
        LogError("Could not load model file: {}", modelName);
        return nullptr;
    }

    auto assetRes = gParser.loadGltf(data.get(), path.parent_path(), fastgltf::Options::GenerateMeshIndices);
    if (assetRes.has_error())
    {
        LogError("Could not load model file: {}", modelName);
        return nullptr;
    }

    fastgltf::Asset const& asset = assetRes.get();

#if !defined(NDEBUG)
    if (auto res = fastgltf::validate(asset); res != fastgltf::Error::None)
    {
        LogError("{}", fastgltf::getErrorMessage(res));
        return nullptr;
    }
#endif
    std::vector<ResourceId> materials;
    auto const&             matSeq = modelYaml["materials"];
    materials.reserve(matSeq.size());

    for (size_t i{0U}; i < matSeq.size(); ++i)
    {
        materials.push_back(utils::StrHash64Runtime(matSeq[i]["id"].as<std::string_view>()));
    }

    auto* model = new Model();
    model->name = modelName;
    model->meshes.resize(asset.meshes.size());

    for (size_t i{0U}; i < asset.meshes.size(); ++i)
    {
        model->meshes[i].name =
            !asset.meshes[i].name.empty() ? asset.meshes[i].name : modelName + "." + std::to_string(i);
        PL_ASSERT(
            ProcessMesh(asset, model->meshes[i], asset.meshes[i], materials), "Sum Tin Wong with mesh processing");
    }

    return std::unique_ptr<Model>(model);
}

}// namespace polos::resource::model
