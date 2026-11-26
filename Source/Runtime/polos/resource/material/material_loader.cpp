///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/material/material_loader.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/utils/string_id.hpp"
#include "polos/utils/yaml_read.hpp"

#include <array>
#include <fstream>
#include <string>

namespace polos::resource::material
{
auto LoadMaterial(std::filesystem::path const& tPath) -> std::optional<rendering::Material>
{
    std::ifstream file(tPath);
    if (!file)
    {
        LogError("Could not open material descriptor: {}", tPath.string());
        return std::nullopt;
    }

    auto const tree         = utils::yaml::Parse(file);
    auto const materialYaml = tree.crootref();

    auto const color   = utils::yaml::ReadArray(utils::yaml::Find(materialYaml, "baseColorFactor"), std::array{1.0F, 1.0F, 1.0F, 1.0F});
    auto const texture = utils::yaml::Scalar(utils::yaml::Find(materialYaml, "baseColorTexture"));

    return rendering::Material{
        .baseColorFactor  = glm::vec4(color[0], color[1], color[2], color[3]),
        .baseColorTexture = texture.empty() ? rendering::kDefaultTexture : utils::StrHash64Runtime(texture),
    };
}
}// namespace polos::resource::material
