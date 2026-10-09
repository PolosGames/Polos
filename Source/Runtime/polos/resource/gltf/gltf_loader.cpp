///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/gltf/gltf_loader.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/resource/model.hpp"
#include "polos/resource/resource_error_domain.hpp"

#include <tiny_gltf_v3.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <limits>
#include <span>
#include <utility>

namespace polos::resource::model
{

namespace
{

struct LoadContext
{
    Model&           model;
    tg3_model const& gltf;
};

struct Semantic
{
    std::string_view name;
    std::int32_t     type;
    std::size_t      offset;
};

constexpr std::array<Semantic, 5U> kSupportedSemantics{
    Semantic{
        .name   = "POSITION",
        .type   = TG3_TYPE_VEC3,
        .offset = offsetof(rendering::PrimitiveVertex, position),
    },
    Semantic{
        .name   = "NORMAL",
        .type   = TG3_TYPE_VEC3,
        .offset = offsetof(rendering::PrimitiveVertex, normal),
    },
    Semantic{
        .name   = "TANGENT",
        .type   = TG3_TYPE_VEC4,
        .offset = offsetof(rendering::PrimitiveVertex, tangent),
    },
    Semantic{
        .name   = "TEXCOORD_0",
        .type   = TG3_TYPE_VEC2,
        .offset = offsetof(rendering::PrimitiveVertex, texCoord),
    },
    Semantic{
        .name   = "COLOR_0",
        .type   = TG3_TYPE_VEC4,
        .offset = offsetof(rendering::PrimitiveVertex, color),
    },
};

struct AccessorData
{
    tg3_accessor const*           accessor;
    std::span<std::uint8_t const> bytes;
    std::size_t                   stride;
    std::size_t                   components;
    std::size_t                   componentSize;
};

// Validate all ranges before forming pointers or allocating destination vertices.
auto GetAccessorData(LoadContext const& tContext, std::int32_t tIndex) -> Result<AccessorData>
{
    auto const& gltf = tContext.gltf;
    if (tIndex < 0 || std::cmp_greater_equal(tIndex, gltf.accessors_count))
    {
        return ErrorType{ResourceErrc::kInvalidGltfAccessorIndex};
    }

    auto const& accessor = gltf.accessors[static_cast<std::size_t>(tIndex)];
    if (accessor.sparse.is_sparse)
    {
        return ErrorType{ResourceErrc::kUnsupportedGltfSparseAccessor};
    }
    if (accessor.buffer_view < 0 || std::cmp_greater_equal(accessor.buffer_view, gltf.buffer_views_count))
    {
        return ErrorType{ResourceErrc::kInvalidGltfBufferViewIndex};
    }

    auto const& view = gltf.buffer_views[static_cast<std::size_t>(accessor.buffer_view)];
    if (view.buffer < 0 || std::cmp_greater_equal(view.buffer, gltf.buffers_count))
    {
        return ErrorType{ResourceErrc::kInvalidGltfBufferIndex};
    }

    auto const& buffer = gltf.buffers[static_cast<std::size_t>(view.buffer)].data;
    if (std::cmp_greater(buffer.count, std::numeric_limits<std::size_t>::max()) || view.byte_offset > buffer.count ||
        view.byte_length > buffer.count - view.byte_offset || accessor.byte_offset > view.byte_length ||
        accessor.count == 0 || buffer.data == nullptr)
    {
        return ErrorType{ResourceErrc::kInvalidGltfBufferRange};
    }

    std::size_t components{0U};
    switch (accessor.type)
    {
        case TG3_TYPE_SCALAR: components = 1U; break;
        case TG3_TYPE_VEC2: components = 2U; break;
        case TG3_TYPE_VEC3: components = 3U; break;
        case TG3_TYPE_VEC4: components = 4U; break;
        default: return ErrorType{ResourceErrc::kUnsupportedGltfAccessorType};
    }

    std::size_t componentSize{0U};
    switch (accessor.component_type)
    {
        case TG3_COMPONENT_TYPE_UNSIGNED_BYTE: componentSize = 1U; break;
        case TG3_COMPONENT_TYPE_UNSIGNED_SHORT: componentSize = 2U; break;
        case TG3_COMPONENT_TYPE_UNSIGNED_INT:
        case TG3_COMPONENT_TYPE_FLOAT: componentSize = 4U; break;
        default: return ErrorType{ResourceErrc::kUnsupportedGltfComponentType};
    }

    std::size_t const   elementSize = components * componentSize;
    std::size_t const   stride      = view.byte_stride != 0 ? view.byte_stride : elementSize;
    std::uint64_t const available   = view.byte_length - accessor.byte_offset;

    // Division avoids overflowing (count - 1) * stride on malformed input.
    if (stride < elementSize || stride % componentSize != 0 || available < elementSize ||
        accessor.count - 1U > (available - elementSize) / stride)
    {
        return ErrorType{ResourceErrc::kInvalidGltfBufferRange};
    }

    auto const offset = view.byte_offset + accessor.byte_offset;
    return AccessorData{
        .accessor      = &accessor,
        .bytes         = std::span{buffer.data + offset, static_cast<std::size_t>(available)},
        .stride        = stride,
        .components    = components,
        .componentSize = componentSize,
    };
}

// glTF components are little-endian; byte reads also avoid alignment/aliasing issues.
auto ReadUnsigned(std::uint8_t const* tBytes, std::size_t tSize) -> std::uint32_t
{
    std::uint32_t value{};
    for (std::size_t i = 0; i < tSize; ++i) value |= static_cast<std::uint32_t>(tBytes[i]) << (i * 8U);
    return value;
}

auto CreateMaterial(LoadContext const& tContext, tg3_primitive const& tPrimitive) -> Result<Material>
{
    // TODO(sorbatdev): Only basic color now, texture later.
    if (-1 != tPrimitive.material)
    {
        if (tPrimitive.material < 0 || std::cmp_greater_equal(tPrimitive.material, tContext.gltf.materials_count))
        {
            LogError("glTF primitive references invalid material index {}", tPrimitive.material);
            return ErrorType{ResourceErrc::kInvalidGltfMaterialIndex};
        }

        auto const& material = tContext.gltf.materials[static_cast<std::size_t>(tPrimitive.material)];
        return Material{
            .baseColor = std::to_array(material.pbr_metallic_roughness.base_color_factor),
            .metallic  = material.pbr_metallic_roughness.metallic_factor,
            .roughness = material.pbr_metallic_roughness.roughness_factor,
        };
    }

    return Material{
        .baseColor = {1.0, 1.0, 1.0, 1.0},
        .metallic  = 1.0,
        .roughness = 1.0,
    };
}

auto LoadPrimitive(LoadContext& tContext, Primitive& tPrimitive, tg3_primitive const& tGltfPrimitive) -> Result<void>
{
    auto const attributes = std::span(tGltfPrimitive.attributes, tGltfPrimitive.attributes_count);
    auto const position   = std::ranges::find_if(attributes, [](auto const& tAttribute) {
        return std::string_view(tAttribute.key.data, tAttribute.key.len) == "POSITION";
    });
    if (position == attributes.end())
    {
        LogError("Position attribute not found in primitive");
        return ErrorType{ResourceErrc::kFailedLoadModel};
    }

    auto const positions = GetAccessorData(tContext, position->value);
    if (!positions.has_value())
    {
        return ErrorType{positions.error()};
    }

    auto const& positionAccessor = *positions->accessor;
    if (positionAccessor.type != TG3_TYPE_VEC3)
    {
        return ErrorType{ResourceErrc::kUnsupportedGltfAccessorType};
    }
    if (positionAccessor.component_type != TG3_COMPONENT_TYPE_FLOAT || positionAccessor.normalized)
    {
        return ErrorType{ResourceErrc::kUnsupportedGltfComponentType};
    }

    Stream stream;
    if (positionAccessor.count > stream.data.max_size())
    {
        return ErrorType{ResourceErrc::kInvalidGltfBufferRange};
    }

    stream.data.resize(static_cast<std::size_t>(positionAccessor.count));
    auto destination = std::as_writable_bytes(std::span{stream.data});

    for (auto const& attribute : attributes)
    {
        std::string_view const name{attribute.key.data, attribute.key.len};
        auto const             semantic = std::ranges::find(kSupportedSemantics, name, &Semantic::name);
        if (semantic == kSupportedSemantics.end())
        {
            LogWarn("Unsupported Vertex attrib: {}", name);
            continue;
        }

        auto const source = GetAccessorData(tContext, attribute.value);
        if (!source.has_value())
        {
            return ErrorType{source.error()};
        }

        auto const& accessor = *source->accessor;
        if (accessor.count != positionAccessor.count)
        {
            return ErrorType{ResourceErrc::kFailedLoadModel};
        }
        if (accessor.type != semantic->type)
        {
            if (name != "COLOR_0" || accessor.type != TG3_TYPE_VEC3)
            {
                return ErrorType{ResourceErrc::kUnsupportedGltfAccessorType};
            }
        }

        bool const isFloat = accessor.component_type == TG3_COMPONENT_TYPE_FLOAT;
        bool const isNormalizedInteger =
            accessor.normalized && (accessor.component_type == TG3_COMPONENT_TYPE_UNSIGNED_BYTE ||
                                    accessor.component_type == TG3_COMPONENT_TYPE_UNSIGNED_SHORT);
        if (isFloat ? accessor.normalized != 0 : !(isNormalizedInteger && (name == "COLOR_0" || name == "TEXCOORD_0")))
        {
            return ErrorType{ResourceErrc::kUnsupportedGltfComponentType};
        }

        for (std::size_t i = 0; i < stream.data.size(); ++i)
        {
            auto const* element = source->bytes.data() + i * source->stride;
            for (std::size_t component = 0; component < source->components; ++component)
            {
                auto const  bits  = ReadUnsigned(element + component * source->componentSize, source->componentSize);
                float const value = isFloat
                                      ? std::bit_cast<float>(bits)
                                      : static_cast<float>(bits) / (source->componentSize == 1U ? 255.0F : 65535.0F);
                if (!std::isfinite(value))
                {
                    return ErrorType{ResourceErrc::kFailedLoadModel};
                }
                std::memcpy(
                    destination.data() + i * sizeof(rendering::PrimitiveVertex) + semantic->offset +
                        component * sizeof(float),
                    &value,
                    sizeof(value));
            }
        }
    }

    IndexStream indexStream;
    bool const  hasIndices = tGltfPrimitive.indices != -1;
    if (hasIndices)
    {
        auto const source = GetAccessorData(tContext, tGltfPrimitive.indices);
        if (!source.has_value())
        {
            return ErrorType{source.error()};
        }

        auto const& accessor = *source->accessor;
        if (accessor.type != TG3_TYPE_SCALAR)
        {
            return ErrorType{ResourceErrc::kUnsupportedGltfAccessorType};
        }
        if (accessor.normalized || accessor.component_type == TG3_COMPONENT_TYPE_FLOAT)
        {
            return ErrorType{ResourceErrc::kUnsupportedGltfComponentType};
        }

        auto const& view = tContext.gltf.buffer_views[static_cast<std::size_t>(accessor.buffer_view)];
        if (view.byte_stride != 0 || std::cmp_greater(accessor.count, indexStream.data.max_size()))
        {
            return ErrorType{ResourceErrc::kInvalidGltfBufferRange};
        }

        indexStream.data.reserve(static_cast<std::size_t>(accessor.count));
        auto const restartIndex = source->componentSize == 4U
                                    ? std::numeric_limits<std::uint32_t>::max()
                                    : (std::uint32_t{1U} << (source->componentSize * 8U)) - 1U;
        for (std::size_t i = 0; i < static_cast<std::size_t>(accessor.count); ++i)
        {
            auto const index = ReadUnsigned(source->bytes.data() + i * source->stride, source->componentSize);
            if (index >= positionAccessor.count || index == restartIndex)
            {
                return ErrorType{ResourceErrc::kFailedLoadModel};
            }
            indexStream.data.push_back(index);
        }
    }

    auto const material = CreateMaterial(tContext, tGltfPrimitive);
    if (!material.has_value())
    {
        return ErrorType{material.error()};
    }

    // Publish only completely decoded primitives.
    tPrimitive.attributes = tContext.model.streams.size();
    tContext.model.streams.push_back(std::move(stream));
    tPrimitive.indices = std::numeric_limits<std::size_t>::max();
    if (hasIndices)
    {
        tPrimitive.indices = tContext.model.indexStreams.size();
        tContext.model.indexStreams.push_back(std::move(indexStream));
    }
    tPrimitive.material = *material;
    return {};
}

auto LoadMesh(LoadContext& tContext, Mesh& tMesh, tg3_mesh const& tGltfMesh) -> Result<void>
{
    tMesh.name = std::string(tGltfMesh.name.data, tGltfMesh.name.len);
    tMesh.primitives.reserve(tGltfMesh.primitives_count);

    for (auto const& primitive : std::span<tg3_primitive const>(tGltfMesh.primitives, tGltfMesh.primitives_count))
    {
        if (auto result = LoadPrimitive(tContext, tMesh.primitives.emplace_back(), primitive); !result.has_value())
        {
            return ErrorType{result.error()};
        }
    }

    return {};
}

auto LogParseErrors(tinygltf3::ErrorStack const& tErrors) -> void
{
    for (std::uint32_t i{0}; i < tErrors.count(); ++i)
    {
        tg3_error_entry const* err = tErrors.entry(i);
        LogError("[{}] {}", static_cast<std::uint32_t>(err->severity), err->message ? err->message : "(null)");
    }
}

}// namespace

auto LoadGltf(std::filesystem::path const& tPath) -> std::unique_ptr<Model>
{
    tinygltf3::Model      tgModel;
    tinygltf3::ErrorStack tgErrors;
    tg3_parse_options     tgOpts;

    tg3_parse_options_init(&tgOpts);

    tg3_error_code parse_ok = tg3_parse_file(
        tgModel.get(),
        tgErrors.get(),
        tPath.c_str(),
        static_cast<std::uint32_t>(tPath.string().size()),
        &tgOpts);

    if (parse_ok != TG3_OK)
    {
        LogError("Failed to parse the glTF model or load its external resources: '{}'", tPath.string());
        LogParseErrors(tgErrors);
        return nullptr;
    }

    auto* model = new Model();
    model->name = std::filesystem::path(tPath).stem().string();
    model->meshes.reserve(tgModel->meshes_count);
    LoadContext context{
        .model = *model,
        .gltf  = *tgModel.get(),
    };

    // 1 pass to determine stream size
    std::size_t streamsSize{0U};
    for (auto const& tgMesh : std::span<tg3_mesh const>(tgModel->meshes, tgModel->meshes_count))
    {
        streamsSize += tgMesh.primitives_count;
    }
    model->streams.reserve(streamsSize);
    model->indexStreams.reserve(streamsSize);

    for (auto const& tgMesh : std::span<tg3_mesh const>(tgModel->meshes, tgModel->meshes_count))
    {
        if (auto res = LoadMesh(context, model->meshes.emplace_back(), tgMesh); !res.has_value())
        {
            LogError("Could not load model '{}': {}", tPath.string(), res.error());
            return nullptr;
        }
    }

    return std::unique_ptr<Model>(model);
}

}// namespace polos::resource::model
