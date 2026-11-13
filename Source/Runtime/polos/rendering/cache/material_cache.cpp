///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/cache/material_cache.hpp"

#include "polos/rendering/cache/texture_cache.hpp"
#include "polos/utils/string_id.hpp"

#include <string>

namespace polos::rendering
{

namespace
{
auto ResolveTexture(size_t tIndex, std::span<resource::model::Texture const> tTextures) -> TextureIndex
{
    if (tIndex >= tTextures.size())
    {
        return kDefaultTextureIndex;
    }

    auto const& texture = tTextures[tIndex];
    return TextureCache::Get().GetOrCreateTextureIndex(texture.resourceId, texture.samplerDetails);
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

auto MaterialCache::Create(CreationDetails const&) -> Result<void>
{
    mMaterials.try_emplace(
        kDefaultMaterial, Material{.name = kDefaultMaterial, .baseColorTexture = kDefaultTextureIndex});
    return {};
}

auto MaterialCache::Destroy() -> void
{
    mMaterials.clear();
    mNextSuffix = 1U;
}

auto MaterialCache::CreateMaterial(
    resource::model::Material const&          tMaterial,
    std::span<resource::model::Texture const> tTextures) -> MaterialHandle
{
    std::string const name   = tMaterial.name.empty() ? "material" : tMaterial.name;
    auto              handle = utils::StrHash64Runtime(name);
    Material          material{
        .name                     = handle,
        .emissiveFactor           = tMaterial.emissiveFactor,
        .alphaMode                = static_cast<AlphaMode>(tMaterial.alphaMode),
        .alphaCutoff              = tMaterial.alphaCutoff,
        .baseColorFactor          = tMaterial.baseColorFactor,
        .baseColorTexture         = ResolveTexture(tMaterial.baseColorTexture, tTextures),
        .metallicFactor           = tMaterial.metallicFactor,
        .roughnessFactor          = tMaterial.roughnessFactor,
        .metallicRoughnessTexture = ResolveTexture(tMaterial.metallicRoughnessTexture, tTextures),
        .normalTexture            = ResolveTexture(tMaterial.normalTexture, tTextures),
        .occlusionTexture         = ResolveTexture(tMaterial.occlusionTexture, tTextures),
        .emissiveTexture          = ResolveTexture(tMaterial.emissiveTexture, tTextures),
    };

    if (auto const* existing = GetMaterial(handle); existing != nullptr && *existing == material)
    {
        return handle;
    }

    while (mMaterials.contains(handle))
    {
        handle = utils::StrHash64Runtime(name + "." + std::to_string(mNextSuffix++));
    }

    material.name = handle;
    mMaterials.emplace(handle, material);
    return handle;
}

auto MaterialCache::CreateMaterial(std::string_view const tName, Material& tMaterial) -> MaterialHandle
{
    std::string const name(tName);
    auto              handle = utils::StrHash64Runtime(name);
    tMaterial.name           = handle;
    if (auto const* existing = GetMaterial(tMaterial.name); existing != nullptr && *existing == tMaterial)
    {
        return handle;
    }

    while (mMaterials.contains(handle))
    {
        handle = utils::StrHash64Runtime(name + "." + std::to_string(mNextSuffix++));
    }

    tMaterial.name = handle;
    mMaterials.emplace(handle, tMaterial);
    return handle;
}

auto MaterialCache::GetMaterial(MaterialHandle tHandle) const -> Material const*
{
    auto const found = mMaterials.find(tHandle);
    return found != mMaterials.end() ? &found->second : nullptr;
}

}// namespace polos::rendering
