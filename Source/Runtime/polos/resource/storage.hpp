///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_STORAGE_HPP
#define POLOS_RESOURCE_STORAGE_HPP

#include "polos/polos_api.hpp"
#include "polos/utils/string_id.hpp"

#include <memory>
#include <unordered_map>
#include <utility>

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

    [[nodiscard]] auto Add(utils::string_id const tKey, Pointer tResource) -> bool
    {
        return nullptr != tResource && mResources.emplace(tKey, std::move(tResource)).second;
    }

    [[nodiscard]] auto Get(utils::string_id const tKey) const -> Pointer
    {
        return Exists(tKey) ? mResources.at(tKey) : nullptr;
    }

    [[nodiscard]] auto Exists(utils::string_id const tKey) const -> bool
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
    friend class ResourceManager;

    std::unordered_map<utils::string_id, Pointer> mResources;
};
}// namespace polos::resource

#endif// POLOS_RESOURCE_STORAGE_HPP
