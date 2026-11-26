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
#include <format>
#include <print>
#include <string_view>
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
    std::string              yamlPath;
    std::vector<std::string> materials;
};

struct MaterialEntry
{
    std::string           id;
    std::string           path;
    std::array<float, 4U> baseColorFactor;
    std::string           baseColorTexture;
};

struct ShaderEntry
{
    std::string id;
    std::string path;
};

std::vector<TextureEntry>  gTextures;
std::vector<MaterialEntry> gMaterials;
std::vector<ModelEntry>    gModels;
std::vector<ShaderEntry>   gShaders;

fastgltf::Parser gParser;

fs::path resourcesDir;

auto AddShaderEntry(fs::path const& tPath, std::string_view tStage) -> void
{
    gShaders.push_back(
        ShaderEntry{
            .id   = std::format("shader.{}.{}", tStage, tPath.stem().string()),
            .path = tPath.lexically_relative(resourcesDir).generic_string() + ".spv",
        });
}

auto AddTextureEntry(fs::path const& tPath) -> void
{
    gTextures.push_back(
        TextureEntry{
            .id   = "texture." + tPath.stem().string(),
            .path = tPath.lexically_relative(resourcesDir).generic_string(),
        });
}

auto ProcessModel(fs::path const& tPath) -> bool
{
    fs::path const modelsPath = "Models";

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
        return false;
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

        if (auto const& baseColor = material.pbrData.baseColorTexture)
        {
            if (baseColor->textureIndex >= asset->textures.size())
            {
                std::println("Invalid base color texture for {}", mat.id);
                return false;
            }
            auto const& texture = asset->textures[baseColor->textureIndex];
            if (!texture.imageIndex || *texture.imageIndex >= asset->images.size())
            {
                std::println("Missing base color image for {}", mat.id);
                return false;
            }
            auto const& image = asset->images[*texture.imageIndex];
            auto const* uri   = std::get_if<fastgltf::sources::URI>(&image.data);
            if (uri == nullptr)
            {
                std::println("Expected an external base color image for {}", mat.id);
                return false;
            }
            auto const imagePath = fs::path(std::string_view(uri->uri->path()));
            mat.baseColorTexture = "texture." + imagePath.stem().string();
        }
        mat.path = "Materials/" + matName + ".yaml";

        modelMaterials.push_back(mat.id);
    }

    fs::path const relativeToModels = tPath.lexically_relative(resourcesDir);

    gModels.push_back(
        ModelEntry{
            .id        = "model." + tPath.stem().string(),
            .path      = relativeToModels.generic_string(),
            .yamlPath  = (modelsPath / tPath.stem()).generic_string() + ".yaml",
            .materials = modelMaterials,
        });

    return true;
}

auto RecurseResources() -> bool
{
    for (auto const& dirEntry : fs::recursive_directory_iterator(resourcesDir))
    {
        if (dirEntry.is_directory())
        {
            continue;
        }

        std::string extension = dirEntry.path().extension().string();
        std::ranges::transform(extension, extension.begin(), [](char const chr) {
            return std::tolower(chr);
        });

        if (extension == ".png" || extension == ".jpg" || extension == ".jpeg")
        {
            AddTextureEntry(dirEntry.path());
        }
        if (extension == ".gltf")
        {
            if (!ProcessModel(dirEntry.path()))
            {
                return false;
            }
        }
        auto const shaderExtension = dirEntry.path().extension();
        if (shaderExtension == ".frag" || shaderExtension == ".vert" || shaderExtension == ".comp" ||
            shaderExtension == ".tesc" || shaderExtension == ".tese" || shaderExtension == ".geom")
        {
            AddShaderEntry(dirEntry.path(), std::string_view(extension).substr(1));
        }
    }

    return true;
}

auto CreateYamlFiles() -> bool
{
    constexpr std::string_view entry = "  - id: {}\n    path: {}";

    constexpr std::string_view materialFile  = "id: {}\nbaseColorFactor: [{}, {}, {}, {}]\nbaseColorTexture: {}\ntilingFactor: 1.0\n";
    constexpr std::string_view materialEntry = "  - id: {}";
    constexpr std::string_view modelFile     = "id: {}\npath: {}\nmaterials:\n{}";

    {
        std::string fileOutput;
        std::format_to(std::back_inserter(fileOutput), "resources:\n");
        for (auto const& texture : gTextures)
        {
            std::format_to(std::back_inserter(fileOutput), entry, texture.id, texture.path);
            std::format_to(std::back_inserter(fileOutput), "\n");
        }
        for (auto const& material : gMaterials)
        {
            std::format_to(std::back_inserter(fileOutput), entry, material.id, material.path);
            std::format_to(std::back_inserter(fileOutput), "\n");
        }
        for (auto const& model : gModels)
        {
            std::format_to(std::back_inserter(fileOutput), entry, model.id, model.yamlPath);
            std::format_to(std::back_inserter(fileOutput), "\n");
        }
        for (auto const& shader : gShaders)
        {
            std::format_to(std::back_inserter(fileOutput), entry, shader.id, shader.path);
            std::format_to(std::back_inserter(fileOutput), "\n");
        }
        std::ofstream(resourcesDir / "resources.yaml", std::ios::trunc) << fileOutput;
    }

    {
        fs::path modelsDir = resourcesDir / "Models";
        if (!gModels.empty() && !fs::exists(modelsDir))
        {
            std::error_code err;
            fs::create_directory(modelsDir, err);

            if (err)
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
            std::format_to(std::back_inserter(modelOut), modelFile, model.id, model.path, materialList);

            fs::path const modelPath = modelsDir / (model.id.substr(model.id.find('.') + 1) + ".yaml");

            std::println("Writing to: {}", modelPath.string());
            std::ofstream(modelPath, std::ios::trunc) << modelOut;
        }
    }

    {
        fs::path materialsDir = resourcesDir / "Materials";
        if (!gMaterials.empty() && !fs::exists(materialsDir))
        {
            std::error_code err;
            fs::create_directory(materialsDir, err);

            if (err)
            {
                std::println("Failed to create Resources/Materials directory.");
                return false;
            }
        }

        for (auto const& material : gMaterials)
        {
            std::ofstream(materialsDir / (material.id.substr(material.id.find('.') + 1) + ".yaml")) << std::format(
                materialFile, material.id, material.baseColorFactor[0], material.baseColorFactor[1],
                material.baseColorFactor[2], material.baseColorFactor[3], material.baseColorTexture);
        }
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

    fs::path const projectDir = tArgv[1];
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
