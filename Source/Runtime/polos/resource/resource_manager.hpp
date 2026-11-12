///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_RESOURCE_MANAGER_HPP
#define POLOS_RESOURCE_RESOURCE_MANAGER_HPP

#include "polos/polos_api.hpp"
#include "polos/resource/storage.hpp"
#include "polos/utils/string_id.hpp"

#include <cstddef>
#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>

namespace polos::fs
{
struct File;
}// namespace polos::fs

namespace polos::resource
{
namespace image
{
struct Image;
}// namespace image

namespace model
{
struct Model;
}// namespace model

using ResourceId = utils::string_id;

/// CPU resource registry and cache. Catalog resources are preloaded before rendering.
/// Catalog paths are relative to "Resources" beside the executable.
class POLOS_API ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();
    /// Explicit resource root for tools and isolated tests.
    explicit ResourceManager(std::filesystem::path const& tResourceRoot);

    ResourceManager(ResourceManager&&)                         = delete;
    auto operator=(ResourceManager&&) -> ResourceManager&      = delete;
    ResourceManager(ResourceManager const&)                    = delete;
    auto operator=(ResourceManager const&) -> ResourceManager& = delete;

    [[nodiscard]] static auto Get() -> ResourceManager&;
    [[nodiscard]] auto        Root() const -> std::filesystem::path const&;

    /// @brief Reads Resources/resources.json and preloads every image, model, and raw file.
    /// Logs the first failure and aborts immediately if the catalog or any resource cannot load.
    auto LoadCatalog() -> void;

    /// @brief Load and cache under tId, or return the cached resource if already loaded.
    [[nodiscard]] auto LoadImage(ResourceId tId, std::filesystem::path const& tPath) -> std::shared_ptr<image::Image>;

    [[nodiscard]] auto LoadImage(ResourceId tId, std::span<std::byte const> tData) -> std::shared_ptr<image::Image>;

    /// @brief Load and cache under tId, or return the cached resource if already loaded.
    [[nodiscard]] auto LoadFile(ResourceId tId, std::filesystem::path const& tPath) -> std::shared_ptr<fs::File>;

    [[nodiscard]] auto LoadFile(ResourceId tId, std::span<std::byte const> tData) -> std::shared_ptr<fs::File>;

    /// Return the cached representation, or nullptr; never load or read the filesystem.
    [[nodiscard]] auto GetImage(ResourceId tId) const -> std::shared_ptr<image::Image>;
    [[nodiscard]] auto GetModel(ResourceId tId) const -> std::shared_ptr<model::Model>;
    [[nodiscard]] auto GetFile(ResourceId tId) const -> std::shared_ptr<fs::File>;

    /// @brief Releases the manager's ownership.
    /// Getters return nullptr until resources are loaded again. Existing shared references remain valid.
    auto ClearCache() -> void;
private:
    [[nodiscard]] auto registerResource(std::string_view tId, std::string_view tPath) -> ResourceId;
    [[nodiscard]] auto normalizePath(std::string_view tPath) const -> std::string;
    auto               loadResources(ResourceManager& tStaged) -> void;

    struct Resource
    {
        std::string path;
    };

    std::filesystem::path                    mRoot;
    std::unordered_map<ResourceId, Resource> mResources;

    Storage<image::Image> mImages;
    Storage<model::Model> mModels;
    Storage<fs::File>     mFiles;
};

}// namespace polos::resource

#endif// POLOS_RESOURCE_RESOURCE_MANAGER_HPP
