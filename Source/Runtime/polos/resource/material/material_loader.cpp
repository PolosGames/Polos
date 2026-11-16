///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/material/material_loader.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/utils/string_id.hpp"

#include <yaml-cpp/yaml.h>

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

    YAML::Node const materialYaml  = YAML::Load(file);
    auto             material      = std::make_shared<Material>();
    material->name                 = utils::StrHash64Runtime(materialYaml["id"].Scalar());
    material->baseColorFactor      = materialYaml["baseColorFactor"].as<decltype(material->baseColorFactor)>();
    material->baseColorTexture.img = utils::StrHash64Runtime(materialYaml["baseColorTexture"].Scalar());

    return material;
}
}// namespace polos::resource::material
