///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/cache/material_cache.hpp"

#include "polos/logging/log_macros.hpp"

namespace polos::rendering
{

MaterialCache* MaterialCache::sInstance = nullptr;

auto MaterialCache::Get() -> MaterialCache&
{
    return *sInstance;
}

MaterialCache::MaterialCache()
{
    sInstance = this;
}

MaterialCache::~MaterialCache()
{
    sInstance = nullptr;
}

auto MaterialCache::Create(CreationDetails const& /*tDetails*/) -> Result<void>
{
    mMaterials.try_emplace(kDefaultMaterial, Material{});

    return {};
}

auto MaterialCache::Destroy() -> void
{
    mMaterials.clear();
}

auto MaterialCache::AddMaterial(ResourceId tId, Material const& tMat) -> ResourceId
{
    auto const [itr, success] = mMaterials.try_emplace(tId, tMat);

    if (!success)
    {
        LogWarn("Trying insert an already existent material.");
    }

    return itr->first;
}

auto MaterialCache::GetMaterial(ResourceId const tId) const -> Material const*
{
    auto const found = mMaterials.find(tId);
    return found != mMaterials.end() ? &found->second : nullptr;
}

}// namespace polos::rendering
