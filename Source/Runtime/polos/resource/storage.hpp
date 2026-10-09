///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_STORAGE_HPP
#define POLOS_RESOURCE_STORAGE_HPP

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
class Storage
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

    /// @return true if inserted, or false if the key already exists.
    [[nodiscard]] auto Add(utils::string_id tKey, Pointer tResource) -> bool
    {
        return tResource && mResources.emplace(tKey, std::move(tResource)).second;
    }

    [[nodiscard]] auto Get(utils::string_id tKey) const -> Pointer
    {
        auto itr = mResources.find(tKey);
        if (itr == mResources.end())
        {
            return nullptr;
        }

        return itr->second;
    }
    /// Exchange cache ownership without making Storage movable.
    auto Swap(Storage& tOther) -> void
    {
        mResources.swap(tOther.mResources);
    }

    auto Clear() -> void
    {
        mResources.clear();
    }
private:
    std::unordered_map<utils::string_id, Pointer> mResources;
};

using ImageStorage = Storage<image::Image>;
using ModelStorage = Storage<model::Model>;
}// namespace polos::resource

#endif// POLOS_RESOURCE_STORAGE_HPP
