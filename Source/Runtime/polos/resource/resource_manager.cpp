///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/resource_manager.hpp"

#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/resource/gltf/gltf_loader.hpp"
#include "polos/resource/image/image_loader.hpp"
#include "polos/resource/material/material_loader.hpp"
#include "polos/resource/model.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/string_map.hpp"

#include "polos/utils/yaml_read.hpp"

#include <cstdlib>
#include <fstream>
#include <system_error>
#include <utility>

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

    enum class ResourceType : uint8_t
    {
        kTexture,
        kMaterial,
        kModel,
    };

    ResourceManager staged(mRoot);
    {
        auto const path = mRoot / "resources.yaml";
        std::ifstream file(path);
        PL_ASSERT(file.is_open(), "Could not open resource catalog: {}", path.string());
        auto const resources = utils::yaml::Parse(file);

        auto const resSeq = utils::yaml::Find(resources.crootref(), "resources");
        PL_ASSERT(resSeq.readable() && resSeq.is_seq(), "Missing resource sequence in {}", path.string());
        for (auto resource : resSeq.children())
        {
            auto const resStr = utils::yaml::Scalar(utils::yaml::Find(resource, "id"));
            ResourceId resId  = staged.registerResource(resStr, utils::yaml::Scalar(utils::yaml::Find(resource, "path")));

            if (resStr.contains("texture."))
            {
                staged.loadImage(resId);
            }
            if (resStr.contains("material."))
            {
                staged.loadMaterial(resId);
            }
            if (resStr.contains("model."))
            {
                staged.loadModel(resId);
            }
        }
    }
    // Resolve model sampler settings after all material descriptors have been loaded.
    for (auto const& [_, model] : staged.mModels.mResources)
    {
        for (auto const& [id, texture] : model->materialTextures)
        {
            auto material = staged.mMaterials.Get(id);
            if (material != nullptr)
            {
                material->baseColorTexture.textureDetails = texture.textureDetails;
                if (material->baseColorTexture.img == 0)
                {
                    material->baseColorTexture.img = texture.img;
                }
            }
        }
    }
    mResources = std::move(staged.mResources);
    mImages.Swap(staged.mImages);
    mMaterials.Swap(staged.mMaterials);
    mModels.Swap(staged.mModels);
    mFiles.Swap(staged.mFiles);
}

auto ResourceManager::LoadImage(ResourceId tId, std::filesystem::path const& tPath) -> std::shared_ptr<image::Image>
{
    if (auto cached = mImages.Get(tId))
    {
        return cached;
    }

    auto loaded = image::LoadImage(tPath);
    return mImages.Add(tId, loaded) ? loaded : nullptr;
}

// auto ResourceManager::LoadImage(ResourceId tId, std::span<std::byte const> tData) -> std::shared_ptr<image::Image>
// {
//     if (auto cached = mImages.Get(tId))
//     {
//         return cached;
//     }

//     auto loaded = image::LoadImage(tData);
//     return mImages.Add(tId, loaded) ? loaded : nullptr;
// }

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

// auto ResourceManager::LoadFile(ResourceId tId, std::span<std::byte const> tData) -> std::shared_ptr<fs::File>
// {
//     if (auto cached = mFiles.Get(tId))
//     {
//         return cached;
//     }

//     auto loaded              = std::make_shared<fs::File>();
//     loaded->uncompressedSize = tData.size();
//     loaded->size             = tData.size();
//     loaded->data.assign(tData.begin(), tData.end());
//     return mFiles.Add(tId, loaded) ? loaded : nullptr;
// }

auto ResourceManager::GetImage(ResourceId const tId) const -> std::shared_ptr<image::Image>
{
    return mImages.Get(tId);
}

auto ResourceManager::GetModel(ResourceId const tId) const -> std::shared_ptr<model::Model>
{
    return mModels.Get(tId);
}

auto ResourceManager::GetMaterial(ResourceId const tId) const -> std::shared_ptr<material::Material>
{
    return mMaterials.Get(tId);
}

auto ResourceManager::GetFile(ResourceId const tId) const -> std::shared_ptr<fs::File>
{
    return mFiles.Get(tId);
}

auto ResourceManager::FetchMaterials() -> std::vector<std::weak_ptr<material::Material>>
{
    std::vector<std::weak_ptr<material::Material>> mats;
    mats.reserve(mMaterials.mResources.size());
    for (auto const& [_, mat] : mMaterials.mResources)
    {
        mats.push_back(mat);
    }

    return mats;
}

auto ResourceManager::FetchModels() -> std::vector<std::weak_ptr<model::Model>>
{
    std::vector<std::weak_ptr<model::Model>> models;
    models.reserve(mModels.mResources.size());
    for (auto const& [_, model] : mModels.mResources)
    {
        models.push_back(model);
    }

    return models;
}

auto ResourceManager::ClearCache() -> void
{
    mImages.Clear();
    mModels.Clear();
    mMaterials.Clear();
    mFiles.Clear();
}

auto ResourceManager::registerResource(std::string_view tId, std::filesystem::path const& tPath) -> ResourceId
{
    auto const resourceId = utils::StrHash64Runtime(tId);
    PL_ASSERT(!mResources.contains(resourceId), "Duplicate resource ID or ID hash collision: {}", tId);

    std::filesystem::path const path = tPath.lexically_normal();

    mResources.emplace(resourceId, Resource{.path = mRoot / path});

    return resourceId;
}


void ResourceManager::loadImage(ResourceId const tResource)
{
    if (mImages.Exists(tResource))
    {
        LogInfo("Image: {} already exists in storage", utils::GetStringFromID(tResource));
        return;
    }

    auto loaded = image::LoadImage(mResources[tResource].path);
    PL_ASSERT(mImages.Add(tResource, loaded), "Failed to load image.");
}

void ResourceManager::loadMaterial(ResourceId const tResource)
{
    if (mMaterials.Exists(tResource))
    {
        LogInfo("Image: {} already exists in storage", utils::GetStringFromID(tResource));
        return;
    }

    auto loaded = material::LoadMaterial(mResources[tResource].path);
    PL_ASSERT(mMaterials.Add(tResource, loaded), "Failed to load material.");
}

void ResourceManager::loadModel(ResourceId const tResource)
{
    if (mModels.Exists(tResource))
    {
        LogInfo("Image: {} already exists in storage", utils::GetStringFromID(tResource));
        return;
    }

    auto loaded = model::LoadGltf(mResources[tResource].path);
    PL_ASSERT(mModels.Add(tResource, std::move(loaded)), "Failed to load model.");
}


}// namespace polos::resource
