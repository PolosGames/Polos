///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/cache/material_cache.hpp"

#include "polos/rendering/cache/texture_cache.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/string_id.hpp"

#include <string>

namespace polos::rendering
{

namespace
{
auto ResolveTexture(resource::material::MaterialTexture const& tTexture) -> TextureIndex
{
    auto index = TextureCache::Get().CreateTexture(tTexture.img, tTexture.textureDetails);
    PL_ASSERT(
        std::numeric_limits<uint32_t>::max() != index,
        "Texture belonging to a material could not be created inside TextureCache");
    return index;
}
}// namespace

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

        mMaterials.try_emplace(
            matPtr->name, Material{
                              .baseColorFactor  = baseColorFactor,
                              .baseColorTexture = ResolveTexture(matPtr->baseColorTexture),
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
