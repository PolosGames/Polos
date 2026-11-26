///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_STORAGE_HPP
#define POLOS_RESOURCE_STORAGE_HPP

#include "polos/polos_api.hpp"
#include "polos/resource/common.hpp"

#include <memory>
#include <unordered_map>
#include <utility>

namespace polos::resource
{

template<typename T>
class POLOS_API Storage
{
public:
    using ResourceType = T;
    using Pointer      = std::shared_ptr<T>;
    using Deleter      = void (*)(ResourceType*);

    Storage()  = default;
    ~Storage() = default;

    Storage(Storage const&)                    = delete;
    Storage(Storage&&)                         = delete;
    auto operator=(Storage const&) -> Storage& = delete;
    auto operator=(Storage&&) -> Storage&      = delete;

    [[nodiscard]] auto Add(ResourceId const tKey, Pointer tResource) -> bool
    {
        return nullptr != tResource && mResources.emplace(tKey, std::move(tResource)).second;
    }

    [[nodiscard]] auto Get(ResourceId const tKey) const -> Pointer
    {
        return Exists(tKey) ? mResources.at(tKey) : nullptr;
    }

    [[nodiscard]] auto Exists(ResourceId const tKey) const -> bool
    {
        return mResources.contains(tKey);
    }

    auto Swap(Storage& tOther) -> void
    {
        mResources.swap(tOther.mResources);
    }

    auto Clear() -> void
    {
        mResources.clear();
    }
private:
    std::unordered_map<ResourceId, Pointer> mResources;
};
}// namespace polos::resource

#endif// POLOS_RESOURCE_STORAGE_HPP
