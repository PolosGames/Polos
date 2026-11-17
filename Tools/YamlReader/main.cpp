///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/utils/yaml_read.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <print>

namespace fs = std::filesystem;

auto main(int32_t const tArgc, char** tArgv) -> int32_t
{
    if (tArgc < 2)
    {
        std::println("No project provided!");
        return 1;
    }

    fs::path const projectDir = tArgv[1];
    if (!fs::exists(projectDir))
    {
        std::println("Project path doesn't exist: {}", projectDir.string());
        return 1;
    }

    fs::path const resourcesDir = projectDir / "Resources";
    if (!fs::exists(resourcesDir))
    {
        std::println("Resources directory doesn't exist: {}", resourcesDir.string());
        return 1;
    }

    fs::path const yamlPath = resourcesDir / "resources.yaml";
    std::ifstream file(yamlPath);
    if (!file)
    {
        std::println("Could not open resource catalog: {}", yamlPath.string());
        return 1;
    }

    auto const tree = polos::utils::yaml::Parse(file);
    auto const resources = polos::utils::yaml::Find(tree.crootref(), "resources");
    if (!resources.readable() || !resources.is_seq())
    {
        std::println("Missing resource sequence in {}", yamlPath.string());
        return 1;
    }

    for (auto resource : resources.children())
    {
        std::println("{}", polos::utils::yaml::Scalar(polos::utils::yaml::Find(resource, "id")));
    }
}
