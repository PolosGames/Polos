///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/resource_manager.hpp"

#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/resource/gltf/gltf_loader.hpp"
#include "polos/resource/image/image_loader.hpp"
#include "polos/resource/model.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/string_map.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <system_error>
#include <unordered_set>
#include <utility>

#if defined(_WIN32)
#    define NOMINMAX
#    include <windows.h>
#elif defined(__APPLE__)
#    include <mach-o/dyld.h>
#endif

namespace polos::resource
{
namespace
{
[[noreturn]] auto AbortCatalog(std::string_view tReason, std::string_view tContext) -> void
{
    LogCritical("Cannot load resource catalog: {} [{}]", tReason, tContext);
    logging::FlushLogger(LOG_CTX_POLOS);
    std::abort();
}

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
// #elif defined(__APPLE__)
//     std::uint32_t size = 0;
//     _NSGetExecutablePath(nullptr, &size);
//     std::string buffer(size, '\0');
//     if (_NSGetExecutablePath(buffer.data(), &size) == 0)
//     {
//         std::error_code error;
//         auto            path = std::filesystem::canonical(buffer.c_str(), error);
//         if (!error)
//             return path.parent_path() / "Resources";
//     }
#endif
    AbortCatalog("Could not determine executable directory", "Resources");
}
}// namespace

ResourceManager::ResourceManager()
    : ResourceManager(ExecutableResourceRoot())
{}

ResourceManager::ResourceManager(std::filesystem::path const& tResourceRoot)
{
    if (!tResourceRoot.empty())
    {
        std::error_code error;
        mRoot = std::filesystem::weakly_canonical(tResourceRoot, error);
        PL_ASSERT(!error, "Could not resolve the Resources directory: {}", tResourceRoot.string());
    }
}

ResourceManager::~ResourceManager() = default;

auto ResourceManager::Instance() -> ResourceManager&
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
    auto          path = normalizePath("resources.json");
    std::ifstream file(mRoot / path);
    PL_ASSERT(static_cast<bool>(file), "Could not open resource catalog: {}", (mRoot / path).string());

    auto const catalog = nlohmann::json::parse(file, nullptr, false);
    PL_ASSERT(catalog.is_array(), "Resource catalog must contain a valid JSON array: {}", (mRoot / path).string());

    ResourceManager                 staged(mRoot);
    std::unordered_set<std::string> paths;
    for (auto const& resource : catalog)
    {
        PL_ASSERT(
            resource.is_object() && resource.contains("id") && resource["id"].is_string() &&
                resource.contains("path") && resource["path"].is_string(),
            "Catalog entries must contain string id and path fields.");

        auto resourceId = staged.registerResource(
            resource["id"].get_ref<std::string const&>(),
            resource["path"].get_ref<std::string const&>());
        auto const& resourcePath = staged.mResources.at(resourceId).path;
        PL_ASSERT(paths.insert(resourcePath).second, "Duplicate normalized resource path: {}", resourcePath);
    }

    // Decode every catalog resource before publishing the new registry and caches.
    // Unknown extensions, including SPIR-V shaders, are loaded as raw files.
    loadResources(staged);

    mResources = std::move(staged.mResources);
    mImages.Swap(staged.mImages);
    mModels.Swap(staged.mModels);
    mFiles.Swap(staged.mFiles);
}

auto ResourceManager::GetImage(ResourceId tId) const -> std::shared_ptr<image::Image>
{
    return mImages.Get(tId);
}

auto ResourceManager::GetModel(ResourceId tId) const -> std::shared_ptr<model::Model>
{
    return mModels.Get(tId);
}

auto ResourceManager::GetFile(ResourceId tId) const -> std::shared_ptr<fs::File>
{
    return mFiles.Get(tId);
}

auto ResourceManager::ClearCache() -> void
{
    mImages.Clear();
    mModels.Clear();
    mFiles.Clear();
}

auto ResourceManager::normalizePath(std::string_view tPath) const -> std::string
{
    PL_ASSERT(!mRoot.empty(), "Resources directory is unavailable.");
    std::string text(tPath);
    std::ranges::replace(text, '\\', '/');

    std::filesystem::path const input(text);
    auto const                  path = input.lexically_normal();

    // Check real ancestors as well, so symlinks cannot escape Resources.
    std::error_code error;
    auto const      absolute = std::filesystem::weakly_canonical(mRoot / path, error);
    PL_ASSERT(!error, "Invalid resource path: {}", tPath);

    auto const relative = absolute.lexically_relative(mRoot);
    PL_ASSERT(
        !relative.empty() && relative != "." && *relative.begin() != "..",
        "Resource path escapes the Resources directory: {}",
        tPath);

    return path.generic_string();
}

auto ResourceManager::registerResource(std::string_view tId, std::string_view tPath) -> ResourceId
{
    PL_ASSERT(!tId.empty() && !tId.contains('\0'), "Resource ID must be nonempty and contain no null characters.");
    auto path = normalizePath(tPath);

    auto const strid = utils::StrHash64(tId);
    // Reject duplicate IDs and hash collisions.
    PL_ASSERT(!mResources.contains(strid), "Duplicate resource ID or ID hash collision: {}", tId);

    auto const registered = utils::GetStringFromID(strid);
    PL_ASSERT(registered.empty() || registered == tId, "Resource ID hash collision: {}", tId);

    std::ignore = utils::RegisterString(tId);
    mResources.emplace(strid, Resource{std::move(path)});
    return strid;
}

auto ResourceManager::loadResources(ResourceManager& tStaged) -> void
{
    for (auto const& [resourceId, resource] : tStaged.mResources)
    {
        auto extension = std::filesystem::path(resource.path).extension().string();
        std::ranges::transform(extension, extension.begin(), [](unsigned char chr) {
            return static_cast<char>(std::tolower(chr));
        });
        auto const resourcePath = tStaged.mRoot / tStaged.normalizePath(resource.path);
        bool       loaded       = false;
        if (extension == ".gltf" || extension == ".glb")
        {
            std::shared_ptr<model::Model> model = model::LoadGltf(resourcePath);
            loaded                              = model && tStaged.mModels.Add(resourceId, std::move(model));
        }
        else if (
            extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp" ||
            extension == ".tga" || extension == ".psd" || extension == ".gif" || extension == ".hdr" ||
            extension == ".pic" || extension == ".pnm" || extension == ".ppm" || extension == ".pgm")
        {
            auto image = image::LoadImage(resourcePath);
            loaded     = image && tStaged.mImages.Add(resourceId, std::move(image));
        }
        else
        {
            auto contents = fs::ReadFile(resourcePath);
            loaded = contents && tStaged.mFiles.Add(resourceId, std::make_shared<fs::File>(std::move(*contents)));
        };

        PL_ASSERT(loaded, "Could not preload resource {} from {}.", utils::GetStringFromID(resourceId), resource.path);
    }
}


}// namespace polos::resource
