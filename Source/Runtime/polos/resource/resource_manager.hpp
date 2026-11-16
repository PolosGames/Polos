///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_RESOURCE_MANAGER_HPP
#define POLOS_RESOURCE_RESOURCE_MANAGER_HPP

#include "polos/polos_api.hpp"
#include "polos/resource/common.hpp"
#include "polos/resource/material.hpp"
#include "polos/resource/storage.hpp"

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

/// CPU resource registry and cache. Catalog resources are preloaded before rendering.
/// Catalog paths are relative to "Resources" beside the executable.
class POLOS_API ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();
    /// Explicit resource root for tools and isolated tests.
    explicit ResourceManager(std::filesystem::path tResourceRoot);

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
    // [[nodiscard]] auto LoadImage(ResourceId tId, std::span<std::byte const> tData) -> std::shared_ptr<image::Image>;

    /// @brief Load and cache under tId, or return the cached resource if already loaded.
    [[nodiscard]] auto LoadFile(ResourceId tId, std::filesystem::path const& tPath) -> std::shared_ptr<fs::File>;
    // [[nodiscard]] auto LoadFile(ResourceId tId, std::span<std::byte const> tData) -> std::shared_ptr<fs::File>;

    /// @brief Return the cached representation, or nullptr
    [[nodiscard]] auto GetImage(ResourceId tId) const -> std::shared_ptr<image::Image>;
    [[nodiscard]] auto GetModel(ResourceId tId) const -> std::shared_ptr<model::Model>;
    [[nodiscard]] auto GetMaterial(ResourceId tId) const -> std::shared_ptr<material::Material>;
    [[nodiscard]] auto GetFile(ResourceId tId) const -> std::shared_ptr<fs::File>;

    [[nodiscard]] auto FetchMaterials() -> std::vector<std::weak_ptr<material::Material>>;
    [[nodiscard]] auto FetchModels() -> std::vector<std::weak_ptr<model::Model>>;

    /// @brief Releases the manager's ownership.
    /// Getters return nullptr until resources are loaded again. Existing shared references remain valid.
    auto ClearCache() -> void;
private:
    auto registerResource(std::string_view tId, std::filesystem::path const& tPath) -> ResourceId;

    void loadImage(ResourceId tResource);
    void loadMaterial(ResourceId tResource);
    void loadModel(ResourceId tResource);

    struct Resource
    {
        std::filesystem::path path;
    };

    std::filesystem::path                    mRoot;
    std::unordered_map<ResourceId, Resource> mResources;

    Storage<image::Image>       mImages;
    Storage<model::Model>       mModels;
    Storage<material::Material> mMaterials;
    Storage<fs::File>           mFiles;
};

}// namespace polos::resource

#endif// POLOS_RESOURCE_RESOURCE_MANAGER_HPP
