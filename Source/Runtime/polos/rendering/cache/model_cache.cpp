///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/cache/model_cache.hpp"

#include <utility>

namespace polos::rendering
{

ModelCache* ModelCache::sInstance = nullptr;

auto ModelCache::Get() -> ModelCache&
{
    return *sInstance;
}

ModelCache::ModelCache()
{
    sInstance = this;
}

ModelCache::~ModelCache()
{
    sInstance = nullptr;
}

auto ModelCache::Create(CreationDetails const&) -> Result<void>
{
    return {};
}

auto ModelCache::Destroy() -> void
{
    mModels.clear();
}

auto ModelCache::AddModel(ResourceId tId, Model tModel) -> void
{
    mModels.try_emplace(tId, std::move(tModel));
}

auto ModelCache::GetModel(ResourceId tId) const -> Model const*
{
    auto const found = mModels.find(tId);
    return found != mModels.end() ? &found->second : nullptr;
}

}// namespace polos::rendering
