///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/resource_manager.hpp"

#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/cache/material_cache.hpp"
#include "polos/rendering/cache/mesh_cache.hpp"
#include "polos/rendering/cache/model_cache.hpp"
#include "polos/rendering/cache/texture_cache.hpp"
#include "polos/rendering/resources/image_data.hpp"
#include "polos/resource/gltf/gltf_loader.hpp"
#include "polos/resource/image/image_loader.hpp"
#include "polos/resource/material/material_loader.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/string_map.hpp"
#include "polos/utils/yaml_read.hpp"

#include <cstdlib>
#include <format>
#include <fstream>
#include <string>
#include <system_error>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#if defined(_WIN32)
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    include <windows.h>
#    undef LoadImage
#endif

namespace polos::resource
{
using std::filesystem::path;

namespace
{

auto ExecutableResourceRoot() -> std::filesystem::path
{
#if defined(_WIN32)
    std::wstring buffer(256, L'\0');
    while (true)
    {
        auto const length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (length == 0)
            break;
        if (length < buffer.size())
            return std::filesystem::path(buffer.substr(0, length)).parent_path() / "Resources";
        buffer.resize(buffer.size() * 2);
    }
#elif defined(__linux__)
    std::error_code error;
    auto            path = std::filesystem::read_symlink("/proc/self/exe", error);
    if (!error)
    {
        return path.parent_path() / "Resources";
    }
#endif
    LogCritical("Cannot load resource catalog: Could not determine executable directory");
    logging::FlushLogger(LOG_CTX_POLOS);
    std::abort();
}
}// namespace

ResourceManager::ResourceManager()
    : ResourceManager(ExecutableResourceRoot())
{}

ResourceManager::ResourceManager(path tResourceRoot)
    : mRoot(std::move(tResourceRoot))
{}

ResourceManager::~ResourceManager() = default;

auto ResourceManager::Get() -> ResourceManager&
{
    static ResourceManager instance;
    return instance;
}

auto ResourceManager::Root() const -> std::filesystem::path const&
{
    return mRoot;
}

auto ResourceManager::LoadCatalog() -> void
{
    PL_ASSERT(!mRoot.empty(), "Resources directory is unavailable.");

    auto const    catalog = mRoot / "resources.yaml";
    std::ifstream file(catalog);
    PL_ASSERT(file.is_open(), "Could not open resource catalog: {}", catalog.string());
    auto const resources = utils::yaml::Parse(file);
    auto const entries   = utils::yaml::Find(resources.crootref(), "resources");
    PL_ASSERT(entries.readable() && entries.is_seq(), "Missing resource sequence in {}", catalog.string());

    std::unordered_set<ResourceId>             ids;
    std::unordered_map<ResourceId, path>       texturePaths;
    std::vector<utils::Pair<ResourceId, path>> materials;
    std::vector<utils::Pair<ResourceId, path>> models;

    for (auto entry : entries.children())
    {
        auto const name         = utils::yaml::Scalar(utils::yaml::Find(entry, "id"));
        auto const relativePath = utils::yaml::Scalar(utils::yaml::Find(entry, "path"));
        PL_ASSERT(!name.empty() && !relativePath.empty(), "Missing ID or path in {}", catalog.string());
        auto const id = utils::RegisterString(name);
        PL_ASSERT(ids.insert(id).second, "Duplicate resource ID or ID hash collision: {}", name);
        auto resourcePath = mRoot / path(relativePath).lexically_normal();

        if (name.starts_with("texture."))
        {
            texturePaths.emplace(id, std::move(resourcePath));
        }
        else if (name.starts_with("material."))
        {
            materials.push_back({.first = id, .second = std::move(resourcePath)});
        }
        else if (name.starts_with("model."))
        {
            models.push_back({.first = id, .second = std::move(resourcePath)});
        }
    }
    mTexturePaths = std::move(texturePaths);

    // Model descriptors supply sampler settings for their cataloged materials.
    std::unordered_map<ResourceId, model::LoadedModel::MaterialTexture> materialTextures;
    for (auto const& [id, modelPath] : models)
    {
        auto& modelsCache = rendering::ModelCache::Get();
        if (modelsCache.GetModel(id) != nullptr)
        {
            continue;
        }

        auto const name   = utils::GetStringFromID(id);
        auto       loaded = model::LoadGltf(modelPath, name);
        PL_ASSERT(loaded != nullptr, "Failed to load model '{}'.", name);
        for (auto const& [materialId, texture] : loaded->materialTextures)
        {
            materialTextures.insert_or_assign(materialId, texture);
        }
        for (auto const& [primitiveId, primitive] : loaded->primitives)
        {
            std::ignore = rendering::MeshCache::Get().AddMesh(primitiveId, primitive);
        }
        modelsCache.AddModel(id, std::move(loaded->model));
    }

    for (auto const& [id, materialPath] : materials)
    {
        if (rendering::MaterialCache::Get().GetMaterial(id) != nullptr)
        {
            continue;
        }

        auto material = material::LoadMaterial(materialPath);
        PL_ASSERT(material.has_value(), "Failed to load material '{}'.", materialPath.string());
        rendering::SamplerDetails sampler{};
        if (auto const found = materialTextures.find(id); found != materialTextures.end())
        {
            sampler = found->second.sampler;
            if (material->baseColorTexture == rendering::kDefaultTexture && found->second.image != 0)
            {
                material->baseColorTexture = found->second.image;
            }
        }
        PL_ASSERT(LoadMaterial(id, *material, sampler) != 0, "Failed to load material '{}'.", materialPath.string());
    }
}

auto ResourceManager::LoadMaterial(ResourceId tId, rendering::Material tMaterial, rendering::SamplerDetails const& tSampler) -> ResourceId
{
    auto& materials = rendering::MaterialCache::Get();
    if (materials.GetMaterial(tId) != nullptr)
    {
        return tId;
    }
    tMaterial.baseColorTexture = loadTexture(tMaterial.baseColorTexture, tSampler);
    if (tMaterial.baseColorTexture == 0)
    {
        return 0;
    }
    return materials.AddMaterial(tId, tMaterial);
}

auto ResourceManager::loadTexture(ResourceId tId, rendering::SamplerDetails const& tSampler) -> ResourceId
{
    if (tId == 0 || tId == rendering::kDefaultTexture)
    {
        return rendering::kDefaultTexture;
    }

    auto& textures  = rendering::TextureCache::Get();
    auto  textureId = tId;
    if (auto const* texture = textures.GetTexture(tId))
    {
        if (texture->details == tSampler)
        {
            return tId;
        }

        // A second sampler for the same image needs its own bindless descriptor.
        textureId = utils::RegisterString(
            std::format(
                "texture-sampler.{}.{}.{}.{}.{}.{}", tId, static_cast<int>(tSampler.minFilter), static_cast<int>(tSampler.magFilter),
                static_cast<int>(tSampler.mipmap), static_cast<int>(tSampler.addressModeU), static_cast<int>(tSampler.addressModeV)));
        if (textures.GetTexture(textureId) != nullptr)
        {
            return textureId;
        }
    }

    auto const found = mTexturePaths.find(tId);
    if (found == mTexturePaths.end())
    {
        LogError("Texture '{}' is missing from the resource catalog.", utils::GetStringFromID(tId));
        return 0;
    }

    auto const image = image::LoadImage(found->second);
    if (image == nullptr)
    {
        return 0;
    }

    VkExtent2D const extent{
        .width  = static_cast<std::uint32_t>(image->width),
        .height = static_cast<std::uint32_t>(image->height),
    };
    return textures.AddTexture(textureId, extent, image->data, tSampler) ? textureId : 0;
}

auto ResourceManager::LoadImage(ResourceId tId, std::filesystem::path const& tPath) -> std::shared_ptr<rendering::ImageData>
{
    if (auto cached = mImages.Get(tId))
    {
        return cached;
    }

    auto loaded = image::LoadImage(tPath);
    return mImages.Add(tId, loaded) ? loaded : nullptr;
}

auto ResourceManager::LoadFile(ResourceId tId, std::filesystem::path const& tPath) -> std::shared_ptr<fs::File>
{
    if (auto cached = mFiles.Get(tId))
    {
        return cached;
    }

    auto contents = fs::ReadFile(tPath);
    if (!contents)
    {
        return nullptr;
    }

    auto loaded = std::make_shared<fs::File>(std::move(*contents));
    return mFiles.Add(tId, loaded) ? loaded : nullptr;
}

auto ResourceManager::GetImage(ResourceId const tId) const -> std::shared_ptr<rendering::ImageData>
{
    return mImages.Get(tId);
}

auto ResourceManager::GetFile(ResourceId const tId) const -> std::shared_ptr<fs::File>
{
    return mFiles.Get(tId);
}

auto ResourceManager::ClearCache() -> void
{
    mTexturePaths.clear();
    mImages.Clear();
    mFiles.Clear();
}

}// namespace polos::resource
