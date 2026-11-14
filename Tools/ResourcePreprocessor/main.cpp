///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include <fastgltf/core.hpp>
#include <fastgltf/math.hpp>
#include <fastgltf/types.hpp>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <print>
#include <vector>

namespace fs = std::filesystem;

struct TextureEntry
{
    std::string id;
    std::string path;
};

struct ModelEntry
{
    std::string              id;
    std::string              path;
    std::vector<std::string> materials;
};

struct MaterialEntry
{
    std::string           id;
    std::string           path;
    std::array<float, 4U> baseColorFactor;
    std::string           baseColorTexture;
};

std::vector<TextureEntry>  gTextures;
std::vector<MaterialEntry> gMaterials;
std::vector<ModelEntry>    gModels;

fastgltf::Parser gParser;

fs::path projectDir;
fs::path resourcesDir;

auto AddTextureEntry(fs::path const& tPath)
{
    std::string const& pathStr = tPath.string();
    gTextures.push_back(
        TextureEntry{
            .id   = "texture." + tPath.stem().string(),
            .path = pathStr.substr(pathStr.find("Resources") + 10U, std::string::npos),
        });
}

auto ProcessModel(fs::path const& tPath) -> bool
{
    auto data = fastgltf::GltfDataBuffer::FromPath(tPath);
    if (!data.has_value())
    {
        std::println("Model data load failed {}", fastgltf::getErrorMessage(data.error()));
        return false;
    }

    auto asset = gParser.loadGltf(data.get(), tPath.parent_path(), fastgltf::Options::None);
    if (!asset.has_value())
    {
        std::println("Model parse failed {}", fastgltf::getErrorMessage(asset.error()));
    }

    size_t                   unnamedMaterialIdx{0};
    std::vector<std::string> modelMaterials;
    for (auto& material : asset->materials)
    {
        MaterialEntry& mat = gMaterials.emplace_back();

        std::string const matName = material.name.empty()
                                      ? tPath.stem().string() + "_Unnamed_" + std::to_string(unnamedMaterialIdx++)
                                      : material.name;
        mat.id                    = "material." + matName;

        for (uint32_t i{0U}; i < mat.baseColorFactor.size(); ++i)
        {
            mat.baseColorFactor[i] = material.pbrData.baseColorFactor[i];
        }

        // Might throw exception
        fastgltf::Image& img =
            asset->images[asset->textures[material.pbrData.baseColorTexture->textureIndex].imageIndex.value()];
        auto& uri = std::get<fastgltf::sources::URI>(img.data);

        std::string_view texturePath = uri.uri->path();
        mat.baseColorTexture         = texturePath.substr(0, texturePath.find('.'));
        mat.path                     = "Materials/" + matName + ".yaml";

        modelMaterials.push_back(mat.id);
    }

    fs::path const relativeToModels = tPath.string().substr(tPath.string().find("Models") + 7U, std::string::npos);

    gModels.push_back(
        ModelEntry{
            .id        = "model." + tPath.stem().string(),
            .path      = relativeToModels.string(),
            .materials = modelMaterials,
        });

    return true;
}

auto RecurseResources() -> bool
{
    for (auto const& dirEntry : fs::recursive_directory_iterator(projectDir))
    {
        if (dirEntry.is_directory())
        {
            continue;
        }

        std::string extension = dirEntry.path().extension();
        std::ranges::transform(extension, extension.begin(), [](char const chr) {
            return std::tolower(chr);
        });

        if (extension == ".png" || extension == ".jpg" || extension == ".jpeg")
        {
            AddTextureEntry(dirEntry.path());
            continue;
        }

        if (extension == ".gltf")
        {
            if (!ProcessModel(dirEntry.path()))
            {
                return false;
            }
        }
    }

    return true;
}

auto CreateYamlFiles() -> bool
{
    constexpr std::string_view texturesCatalogue  = "textures:";
    constexpr std::string_view materialsCatalogue = "materials:";
    constexpr std::string_view modelsCatalogue    = "models:";

    constexpr std::string_view entry = "  - id: {}\n    path: {}";

    constexpr std::string_view materialFile  = "id: {}\nbaseColorFactor: [{}, {}, {}, {}]\nbaseColorTexture: {}\n";
    constexpr std::string_view materialEntry = "  - id: {}";
    constexpr std::string_view modelFile     = "id: {}\nmaterials:\n{}";

    std::string fileOutput;
    std::format_to(std::back_inserter(fileOutput), "{}\n", texturesCatalogue);

    for (auto const& texture : gTextures)
    {
        std::format_to(std::back_inserter(fileOutput), entry, texture.id, texture.path);
        std::format_to(std::back_inserter(fileOutput), "\n");
    }

    std::format_to(std::back_inserter(fileOutput), "{}\n", materialsCatalogue);
    for (auto const& material : gMaterials)
    {
        std::format_to(std::back_inserter(fileOutput), entry, material.id, material.path);
        std::format_to(std::back_inserter(fileOutput), "\n");
    }

    std::format_to(std::back_inserter(fileOutput), "{}\n", modelsCatalogue);
    for (auto const& model : gModels)
    {
        std::format_to(std::back_inserter(fileOutput), entry, model.id, model.path);
        std::format_to(std::back_inserter(fileOutput), "\n");
    }

    {
        fs::path modelsDir = resourcesDir / "Models";
        if (!gModels.empty() && !fs::exists(modelsDir))
        {
            std::error_code err;
            fs::create_directory(modelsDir, err);

            if (!err)
            {
                std::println("Failed to create Resources/Models directory.");
                return false;
            }
        }

        for (auto const& model : gModels)
        {
            std::string materialList;
            for (auto const& material : model.materials)
            {
                std::format_to(std::back_inserter(materialList), materialEntry, material);
                std::format_to(std::back_inserter(materialList), "\n");
            }

            std::string modelOut;
            std::format_to(std::back_inserter(modelOut), modelFile, model.id, materialList);

            std::ofstream(modelsDir / model.id / ".yaml", std::ios::trunc) << modelOut;
        }
    }

    {
        std::ofstream(resourcesDir / "resources.yaml", std::ios::trunc) << fileOutput;
    }

    return true;
}

auto ProcessResources() -> int32_t
{
    if (!RecurseResources())
    {
        return 2;
    }
    if (!CreateYamlFiles())
    {
        return 3;
    }

    return 0;
}

auto main(int32_t const tArgc, char** tArgv) -> int32_t
{
    if (tArgc < 2)
    {
        std::println("No project provided!");
        return 1;
    }

    std::error_code error;
    fs::path        path = fs::read_symlink("/proc/self/exe", error);

    std::string pathStr = path.string();
    std::ranges::replace(pathStr, '\\', '/');

    size_t polosDirIndex = pathStr.find("Polos");
    pathStr              = pathStr.substr(0, polosDirIndex + 6U);

    projectDir = pathStr + tArgv[1];
    if (!fs::exists(projectDir))
    {
        std::println("Project path doesn't exist: {}", projectDir.string());
        return 1;
    }
    resourcesDir = projectDir / "Resources";
    if (!fs::exists(resourcesDir))
    {
        std::println("Resources directory doesn't exist: {}", resourcesDir.string());
        return 1;
    }

    return ProcessResources();
}
