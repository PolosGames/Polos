///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_RESOURCE_MANAGER_HPP
#define POLOS_RESOURCE_RESOURCE_MANAGER_HPP

#include "polos/polos_api.hpp"
#include "polos/resource/common.hpp"
#include "polos/resource/storage.hpp"

#include <filesystem>
#include <memory>
#include <unordered_map>

namespace polos::fs
{
struct File;
}// namespace polos::fs

namespace polos::rendering
{
struct ImageData;
struct Material;
struct SamplerDetails;
}// namespace polos::rendering

namespace polos::resource
{

/// Loads cataloged rendering resources into the rendering caches.
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

    /// @brief Reads Resources/resources.yaml after the rendering caches are initialized.
    /// Loads models, materials and their referenced textures. Aborts if a required asset fails to load.
    auto LoadCatalog() -> void;

    /// @brief Registers a material and loads its texture from the catalog. Returns zero on failure.
    [[nodiscard]] auto LoadMaterial(ResourceId tId, rendering::Material tMaterial, rendering::SamplerDetails const& tSampler) -> ResourceId;

    /// @brief Load and cache CPU data under tId, or return the cached data if already loaded.
    [[nodiscard]] auto LoadImage(ResourceId tId, std::filesystem::path const& tPath) -> std::shared_ptr<rendering::ImageData>;
    [[nodiscard]] auto LoadFile(ResourceId tId, std::filesystem::path const& tPath) -> std::shared_ptr<fs::File>;

    [[nodiscard]] auto GetImage(ResourceId tId) const -> std::shared_ptr<rendering::ImageData>;
    [[nodiscard]] auto GetFile(ResourceId tId) const -> std::shared_ptr<fs::File>;

    /// @brief Releases the manager's catalog paths and CPU data. The renderer owns its caches.
    auto ClearCache() -> void;
private:
    [[nodiscard]] auto loadTexture(ResourceId tId, rendering::SamplerDetails const& tSampler) -> ResourceId;

    std::filesystem::path                                 mRoot;
    std::unordered_map<ResourceId, std::filesystem::path> mTexturePaths;
    Storage<rendering::ImageData>                         mImages;
    Storage<fs::File>                                     mFiles;
};

}// namespace polos::resource

#endif// POLOS_RESOURCE_RESOURCE_MANAGER_HPP
