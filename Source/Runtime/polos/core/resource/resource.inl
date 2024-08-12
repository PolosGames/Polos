#pragma once

#include "polos/core/resource/resource_common.h"
#include "polos/core/resource/resource_cache.h"

namespace polos::resource
{
    template<typename T>
    auto ImportResource(std::string const& p_Path, import_options<T> p_Options) -> ResourceHandle
    {
        return ResourceCache<T>::ImportResource(p_Path, std::forward<import_options<T>>(p_Options));
    }

    template<typename T>
    auto GetResource(std::string const& p_Name) -> ResourceHandle
    {
        uint32 hash = StrHash32(p_Name.c_str());
        auto handle = ResourceCache<T>::GetResource(hash);

        if (handle == INVALID_RESOURCE)
            LOG_ENGINE_ERROR("[GetResource] Resource with the name \"{}\" was not found", p_Name);

        return handle;
    }

    template<typename T>
    auto GetRawResource(std::string const& p_Name) -> T*
    {
        uint32 hash = StrHash32(p_Name.c_str());
        auto* ptr = ResourceCache<T>::GetRawResource(hash);

        if (ptr == nullptr)
            LOG_ENGINE_ERROR("[GetRawResource] Resource with the name \"{}\" was not found", p_Name);

        return ptr;
    }

    template<typename T>
    auto DestroyResource(std::string const& p_Name) -> void
    {
        uint32 hash = StrHash32(p_Name.c_str());
        auto result = ResourceCache<T>::DestroyResource(hash);

        if(!result)
            LOG_ENGINE_ERROR("[DestroyResource] Resource with the name \"{}\" was not found", p_Name);
    }
} // namespace polos::resource
