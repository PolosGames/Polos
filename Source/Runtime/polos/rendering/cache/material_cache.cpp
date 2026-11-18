///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/cache/material_cache.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/cache/texture_cache.hpp"

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

    std::vector<std::weak_ptr<resource::material::Material>> materials =
        resource::ResourceManager::Get().FetchMaterials();

    for (auto& material : materials)
    {
        auto matPtr = material.lock();

        mMaterials.try_emplace(matPtr->name, Material::Create(*matPtr));
    }

    return {};
}

auto MaterialCache::Destroy() -> void
{
    mMaterials.clear();
    mNextSuffix = 1U;
}

auto MaterialCache::AddMaterial(resource::ResourceId tName, Material const& tMat) -> MaterialHandle
{
    auto const [itr, success] = mMaterials.try_emplace(tName, tMat);

    if (!success)
    {
        LogWarn("Trying insert an already existent material.");
    }

    return itr->first;
}

auto MaterialCache::GetMaterial(MaterialHandle const tHandle) const -> Material const*
{
    auto const found = mMaterials.find(tHandle);
    return found != mMaterials.end() ? &found->second : nullptr;
}

}// namespace polos::rendering
