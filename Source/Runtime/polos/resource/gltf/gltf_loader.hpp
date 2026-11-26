///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_GLTF_LOADER_HPP
#define POLOS_RESOURCE_GLTF_LOADER_HPP


#include "polos/rendering/resources/model.hpp"
#include "polos/rendering/resources/texture.hpp"
#include "polos/utils/pair.hpp"

#include <filesystem>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace polos::resource::model
{

struct LoadedModel
{
    struct MaterialTexture
    {
        ResourceId                image{0};
        rendering::SamplerDetails sampler{};
    };

    rendering::Model                                                   model;
    std::vector<utils::Pair<ResourceId, rendering::MeshPrimitiveData>> primitives;
    std::unordered_map<ResourceId, MaterialTexture>                    materialTextures;
};

/// @return The model hierarchy and upload data, or nullptr if parsing or validation fails.
[[nodiscard]] auto LoadGltf(std::filesystem::path const& tPath, std::string_view tId = {}) -> std::unique_ptr<LoadedModel>;

}// namespace polos::resource::model

#endif// POLOS_RESOURCE_GLTF_LOADER_HPP
