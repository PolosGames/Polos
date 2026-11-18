///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/material/material_loader.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/utils/string_id.hpp"
#include "polos/utils/yaml_read.hpp"

#include <fstream>
#include <string>

namespace polos::resource::material
{
auto LoadMaterial(std::filesystem::path const& tPath) -> std::shared_ptr<Material>
{
    std::ifstream file(tPath);
    if (!file)
    {
        LogError("Could not open material descriptor: {}", tPath.string());
        return nullptr;
    }

    auto const tree         = utils::yaml::Parse(file);
    auto const materialYaml = tree.crootref();

    auto material  = std::make_shared<Material>();
    material->name = utils::StrHash64Runtime(utils::yaml::Scalar(utils::yaml::Find(materialYaml, "id")));
    material->baseColorFactor =
        utils::yaml::ReadArray(utils::yaml::Find(materialYaml, "baseColorFactor"), material->baseColorFactor);
    auto const texture = utils::yaml::Scalar(utils::yaml::Find(materialYaml, "baseColorTexture"));
    if (!texture.empty())
    {
        material->baseColorTexture.img = utils::StrHash64Runtime(texture);
    }

    return material;
}
}// namespace polos::resource::material
