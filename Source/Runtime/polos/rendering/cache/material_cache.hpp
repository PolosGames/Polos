///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_CACHE_MATERIAL_CACHE_HPP
#define POLOS_RENDERING_CACHE_MATERIAL_CACHE_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/resources/material.hpp"

#include <unordered_map>

namespace polos::rendering
{

class POLOS_API MaterialCache
{
public:
    [[nodiscard]] static auto Get() -> MaterialCache&;

    struct CreationDetails
    {
    };

    MaterialCache();
    ~MaterialCache();

    auto Create(CreationDetails const&) -> Result<void>;
    auto Destroy() -> void;

    MaterialCache(MaterialCache const&)            = delete;
    MaterialCache(MaterialCache&&)                 = delete;
    MaterialCache& operator=(MaterialCache const&) = delete;
    MaterialCache& operator=(MaterialCache&&)      = delete;

    [[nodiscard]] auto AddMaterial(ResourceId tId, Material const& tMat) -> ResourceId;
    [[nodiscard]] auto GetMaterial(ResourceId tId) const -> Material const*;
private:
    static MaterialCache* sInstance;

    std::unordered_map<ResourceId, Material> mMaterials;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_CACHE_MATERIAL_CACHE_HPP
