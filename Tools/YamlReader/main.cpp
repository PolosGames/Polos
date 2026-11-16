///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include <yaml-cpp/yaml.h>

#include <algorithm>
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

    std::error_code error;
    fs::path const  path = fs::read_symlink("/proc/self/exe", error);

    std::string pathStr = path.string();
    std::ranges::replace(pathStr, '\\', '/');

    size_t polosDirIndex = pathStr.find("Polos");
    pathStr              = pathStr.substr(0, polosDirIndex + 6U);

    fs::path const projectDir = pathStr + tArgv[1];

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

    static auto const typeToStr = [](auto const tType) -> std::string {
        switch (tType)
        {
            case YAML::NodeType::Null: return "Null";
            case YAML::NodeType::Scalar: return "Scalar";
            case YAML::NodeType::Sequence: return "Seq";
            case YAML::NodeType::Map: return "Map";
            case YAML::NodeType::Undefined: return "Undef";
        }
    };

    auto resSeq = resources["resources"];
    for (uint32_t i{0U}; i < resSeq.size(); ++i)
    {
        std::println("{}", resSeq[i]["id"].as<std::string_view>());
    }
}
