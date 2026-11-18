///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/cache/material_cache.hpp"

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

        glm::vec4 baseColorFactor;
        for (uint32_t i{0U}; i < matPtr->baseColorFactor.size(); ++i)
        {
            baseColorFactor[static_cast<int32_t>(i)] = matPtr->baseColorFactor[i];
        }

        auto const& texture = matPtr->baseColorTexture;
        mMaterials.try_emplace(
            matPtr->name, Material{
                              .baseColorFactor  = baseColorFactor,
                              .baseColorTexture = TextureCache::Get().GetTextureIndex(texture.img, texture.textureDetails),
                          });
    }

    return {};
}

auto MaterialCache::Destroy() -> void
{
    mMaterials.clear();
    mNextSuffix = 1U;
}

auto MaterialCache::GetMaterial(MaterialHandle const tHandle) const -> Material const*
{
    auto const found = mMaterials.find(tHandle);
    return found != mMaterials.end() ? &found->second : nullptr;
}

}// namespace polos::rendering
