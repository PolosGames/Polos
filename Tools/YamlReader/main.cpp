///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include <yaml-cpp/yaml.h>

#include <cstdint>
#include <filesystem>
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

    fs::path const yamlPath  = resourcesDir / "resources.yaml";
    YAML::Node     resources = YAML::LoadFile(yamlPath.string());

    auto resSeq = resources["resources"];
    for (uint32_t i{0U}; i < resSeq.size(); ++i)
    {
        std::println("{}", resSeq[i]["id"].as<std::string_view>());
    }
}
